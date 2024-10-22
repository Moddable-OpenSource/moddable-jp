/*
 * Copyright (c) 2024 Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Runtime.
 *
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *  
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.  
 *
 */

#include "xsmc.h"
#include "mc.xs.h"
#include "mc.defines.h"

#include "builtinCommon.h"

#include "driver/i2s_pdm.h"
#include "driver/i2s_std.h"
#include "freertos/task.h"

#ifdef MODDEF_AUDIOOUT_AMPLIFIER_POWER
	#include "modGPIO.h"
#endif

#ifndef MODDEF_AUDIOOUT_I2S_SLOT
	#define MODDEF_AUDIOOUT_I2S_SLOT I2S_STD_SLOT_RIGHT
#endif

#include "xsHost.h"

#ifdef MODDEF_AUDIOOUT_I2S_PDM_PIN
	uint8_t gPDMAudioOutBusy = 0;
	uint8_t gPDMAudioInBusy __attribute__((weak)) = 0;
#endif

typedef struct AudioOutElementRecord AudioOutElementRecord;
typedef struct AudioOutElementRecord *AudioOutElement;

struct AudioOutElementRecord {
	AudioOutElement		next;

	xsSlot				*completion;
	xsSlot				*buffer;
	void				*data;		// NULL if relocatable
	
	uint32_t			position;
	uint32_t			bytesAvailable;
	
	esp_err_t			err;
};

typedef struct AudioOutRecord AudioOutRecord;
typedef struct AudioOutRecord *AudioOut;

struct AudioOutRecord {
	xsSlot				obj;
	uint32_t			total_dma_buf_size;
	uint32_t			dma_buf_size;
	uint32_t			bytesWritable;
	uint8_t				useCount;
	uint8_t				started;
	uint8_t				callbackPending;

	uint16_t			sampleRate;
	uint8_t				numChannels;
	uint8_t				bitsPerSample;

	i2s_chan_handle_t	tx_handle;

	xsMachine			*the;
	xsSlot				*onWritable;

	double				volumeDouble;
	int16_t				volumeFixed;
	
	AudioOutElement		elements;
	volatile TaskHandle_t		task;

#ifdef MODDEF_AUDIOOUT_AMPLIFIER_POWER
	modGPIOConfigurationRecord	amplifierPower;
	int						bytesInFlight;
#endif
};

static bool playedBuffer(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx);
static esp_err_t doWrite(AudioOut audioOut, void *buffer, xsUnsignedValue bytes);
static void audiooutDeliver(void *theIn, void *refcon, uint8_t *message, uint16_t messageLength);
static void xs_audioout_mark_(xsMachine* the, void* it, xsMarkRoot markRoot);
static void audioOutLoop(void *pvParameter);

static const xsHostHooks ICACHE_RODATA_ATTR xsAudioOutHooks = {
	xs_audioout_destructor_,
	xs_audioout_mark_,
	C_NULL
};

static void audioOutRelease(AudioOut audioOut)
{
	if (audioOut->task) {
		xTaskNotify(audioOut->task, 2, eSetValueWithOverwrite);	// end task
		while (audioOut->task)
			modDelayMilliseconds(1);
	}

#ifdef MODDEF_AUDIOOUT_I2S_PDM_PIN
	if (audioOut->tx_handle) {
		i2s_channel_disable(audioOut->tx_handle);
		i2s_del_channel(audioOut->tx_handle);
		audioOut->tx_handle = NULL;
	}
	gPDMAudioOutBusy = 0;
#endif

	while (audioOut->elements) {
		AudioOutElement element = audioOut->elements, next = element->next;
		c_free(element);
		audioOut->elements = next;
	}
}

void xs_audioout_destructor_(void *data)
{
	AudioOut audioOut = data;
	if (!audioOut) return;
	
	audioOutRelease(audioOut);
	c_free(audioOut);
}

void xs_audioout_constructor_(xsMachine *the)
{
	AudioOut audioOut;
	xsSlot *onWritable;
	esp_err_t err;
	int sampleRate = 0;
	int numChannels = 0;
	int bitsPerSample = 0;

	xsmcVars(1);

	if (xsmcHas(xsArg(0), xsID_sampleRate)) {
		xsmcGet(xsVar(0), xsArg(0), xsID_sampleRate);
		sampleRate = xsmcToInteger(xsVar(0));
	}
#ifdef MODDEF_AUDIOOUT_SAMPLERATE
	else
		sampleRate = MODDEF_AUDIOOUT_SAMPLERATE;
#endif
	if ((sampleRate < 8000) || (sampleRate > 48000))
		xsRangeError("invalid sample rate");

	if (xsmcHas(xsArg(0), xsID_channels)) {
		xsmcGet(xsVar(0), xsArg(0), xsID_channels);
		numChannels = xsmcToInteger(xsVar(0));
	}
#ifdef MODDEF_AUDIOOUT_NUMCHANNELS
	else
		numChannels = MODDEF_AUDIOOUT_NUMCHANNELS;
#endif
	if ((1 != numChannels) && (2 != numChannels))
		xsRangeError("bad numChannels");

	if (xsmcHas(xsArg(0), xsID_bitsPerSample)) {
		xsmcGet(xsVar(0), xsArg(0), xsID_bitsPerSample);
		bitsPerSample = xsmcToInteger(xsVar(0));
	}
#ifdef MODDEF_AUDIOOUT_BITSPERSAMPLE
	else
		bitsPerSample = MODDEF_AUDIOOUT_BITSPERSAMPLE;
#endif
	if ((8 != bitsPerSample) && (16 != bitsPerSample))
		xsRangeError("bad bitsPerSample");

	onWritable = builtinGetCallback(the, xsID_onWritable);

	builtinInitializeTarget(the);

	if (kIOFormatBuffer != builtinInitializeFormat(the, kIOFormatBuffer))
		xsRangeError("invalid format");

	audioOut = c_calloc(1, sizeof(AudioOutRecord));
	if (!audioOut)
		xsRangeError("no memory");

	xsmcSetHostData(xsThis, audioOut);

	audioOut->the = the;
	audioOut->obj = xsThis;

	audioOut->useCount = 1;

	audioOut->sampleRate = (uint16_t)sampleRate;
	audioOut->numChannels = (uint8_t)numChannels;
	audioOut->bitsPerSample = (uint8_t)bitsPerSample;

	audioOut->onWritable = onWritable;
	
	audioOut->volumeDouble = 1.0;
	audioOut->volumeFixed = 256;

	xsSetHostHooks(xsThis, (xsHostHooks *)&xsAudioOutHooks);
	xsRemember(audioOut->obj);

    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;

	// number of DMA buffers and their size in samples (default is 6 and 240)
//	tx_chan_cfg.dma_desc_num = 6;
//    tx_chan_cfg.dma_frame_num = 480;		// maximum of 4096

#ifdef MODDEF_AUDIOOUT_I2S_PDM_PIN
	if (gPDMAudioOutBusy)
		xsUnknownError("already in use");

	if (gPDMAudioInBusy)
		xsUnknownError("pdm already in use by audioin");

	i2s_pdm_tx_config_t tx_cfg = {
		.clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(audioOut->sampleRate),
		.slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
		.gpio_cfg = {
			.clk = -1,
			.dout = MODDEF_AUDIOOUT_I2S_PDM_PIN,
			.invert_flags = {
				.clk_inv = false,
			},
		},
	};
#elif MODDEF_AUDIOOUT_I2S_BCK_PIN
	i2s_std_config_t i2s_config = {
		.gpio_cfg = {
			.mclk = I2S_GPIO_UNUSED,
			.bclk = MODDEF_AUDIOOUT_I2S_BCK_PIN,
			.ws = MODDEF_AUDIOOUT_I2S_LR_PIN,
			.dout = MODDEF_AUDIOOUT_I2S_DATAOUT_PIN,
			.din = I2S_GPIO_UNUSED,
			.invert_flags = {
				.mclk_inv = false,
				.bclk_inv = false,
				.ws_inv = false,
			},
		},
	};

	// I2S_STD_CLK_DEFAULT_CONFIG(sampleRate)  (i2s_std.h)
	i2s_config.clk_cfg.sample_rate_hz = audioOut->sampleRate;
	i2s_config.clk_cfg.clk_src = I2S_CLK_SRC_DEFAULT;
	i2s_config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;

	// I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bitwidth, mode) (i2s_std.h)
	int msb_right = true;
#if MODDEF_AUDIOOUT_I2S_BITSPERSAMPLE == 32
	i2s_config.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
	i2s_config.slot_cfg.ws_width = I2S_DATA_BIT_WIDTH_32BIT;
	msb_right = false;
#elif MODDEF_AUDIOOUT_I2S_BITSPERSAMPLE == 16
	i2s_config.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
	i2s_config.slot_cfg.ws_width = I2S_DATA_BIT_WIDTH_16BIT;
#else
	i2s_config.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_8BIT;
	i2s_config.slot_cfg.ws_width = I2S_DATA_BIT_WIDTH_8BIT;
#endif
	i2s_config.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO;
#if SOC_I2S_HW_VERSION_1	// esp32/s2
	i2s_config.slot_cfg.msb_right = msb_right;
#else
	i2s_config.slot_cfg.left_align = false;
	i2s_config.slot_cfg.big_endian = false;
	i2s_config.slot_cfg.bit_order_lsb = false;
#endif

#if MODDEF_AUDIOOUT_NUMCHANNELS == 2
	i2s_config.slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
	i2s_config.slot_cfg.slot_mask = I2S_SLOT_MODE_BOTH;
#else
	i2s_config.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO;
	i2s_config.slot_cfg.slot_mask = MODDEF_AUDIOOUT_I2S_SLOT;
#endif
	i2s_config.slot_cfg.ws_pol = false;
	i2s_config.slot_cfg.bit_shift = false;


#elif MODDEF_AUDIOOUT_I2S_DAC
	#error DAC audio unimplemented
#else
	#error unknown audio configuration
#endif

	err = i2s_new_channel(&tx_chan_cfg, &audioOut->tx_handle, C_NULL);
	if (ESP_OK != err)
		xsUnknownError("new channel failed");
#if MODDEF_AUDIOOUT_I2S_PDM_PIN
	err = i2s_channel_init_pdm_tx_mode(audioOut->tx_handle, &tx_cfg);
	if (ESP_OK != err)
		xsUnknownError("init PDM failed");
	gPDMAudioOutBusy = 1;
#elif MODDEF_AUDIOOUT_I2S_BCK_PIN
	err = i2s_channel_init_std_mode(audioOut->tx_handle, &i2s_config);
	i2s_channel_reconfig_std_slot(audioOut->tx_handle, &i2s_config.slot_cfg);
	i2s_channel_reconfig_std_clock(audioOut->tx_handle, &i2s_config.clk_cfg);

#elif MODDEF_AUDIOOUT_I2S_DAC
#else
#endif

	i2s_event_callbacks_t cbs = {
		.on_recv = C_NULL,
		.on_recv_q_ovf = C_NULL,
		.on_sent = playedBuffer,
		.on_send_q_ovf = C_NULL,
	};
	i2s_channel_register_event_callback(audioOut->tx_handle, &cbs, audioOut);

	audioOut->dma_buf_size = tx_chan_cfg.dma_frame_num * 2;
	audioOut->total_dma_buf_size = tx_chan_cfg.dma_frame_num * tx_chan_cfg.dma_desc_num * 2;		//@@ wrong for stereo etc
	audioOut->bytesWritable = audioOut->total_dma_buf_size; 

#if ESP32 && defined(MODDEF_AUDIOOUT_AMPLIFIER_POWER)
	modGPIOInit(&audioOut->amplifierPower, C_NULL, MODDEF_AUDIOOUT_AMPLIFIER_POWER, kModGPIOOutput);
	modGPIOWrite(&audioOut->amplifierPower, 1);		//@@ always on
#endif

	if (!audioOut->callbackPending && audioOut->onWritable) {
		audioOut->callbackPending = true;
		audioOut->useCount += 1;		//@@ atomic
		modMessagePostToMachine(audioOut->the, C_NULL, 0, audiooutDeliver, audioOut);
	}
}
 
void xs_audioout_close_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostData(xsThis);
	if (audioOut && xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks)) {
		xsForget(audioOut->obj);
		
#if defined(_NO_ATOMICS)
		uint8_t useCount = --audioOut->useCount;
#else
		uint8_t useCount = __atomic_sub_fetch(&audioOut->useCount, 1, __ATOMIC_SEQ_CST);
#endif
		audioOutRelease(audioOut);
		if (0 == audioOut->useCount)
			xs_audioout_destructor_(audioOut);
		xsmcSetHostData(xsThis, C_NULL);
		xsmcSetHostDestructor(xsThis, C_NULL);
	}
}
 
void xs_audioout_start_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	esp_err_t err;

	if (audioOut->started)
		xsUnknownError("already started");

	err = i2s_channel_enable(audioOut->tx_handle);
	if (ESP_OK != err)
		xsUnknownError("can't enable");

	audioOut->started = true;
	
	if (!audioOut->callbackPending /* && audioOut->onWritable */) {
		audioOut->callbackPending = true;
		audioOut->useCount += 1;		//@@ atomic
		modMessagePostToMachine(audioOut->the, C_NULL, 0, audiooutDeliver, audioOut);
	}
}
 
void xs_audioout_stop_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);

	if (!audioOut->started)
		return;

	i2s_channel_disable(audioOut->tx_handle);
	audioOut->started = false;
	audioOut->bytesWritable = audioOut->total_dma_buf_size; 
}
 
void xs_audioout_writeSync_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	void *buffer;
	xsUnsignedValue requested;
	esp_err_t err;

	xsmcGetBufferReadable(xsArg(0), &buffer, &requested);

	if (requested > audioOut->bytesWritable)
		xsUnknownError("insufficient space");

	if (requested & 1)							//@@ broken for stereo & 8 bit
		xsUnknownError("full samples only");

	err = doWrite(audioOut, buffer, requested);
	if (err)
		xsUnknownError("write failed");
}
 
 void xs_audioout_writeAsync_(xsMachine *the)
 {
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	AudioOutElement element;
	xsSlot *buffer, *completion = C_NULL;
	void *data;
	xsUnsignedValue bytesAvailable;

	if (xsBufferRelocatable == xsmcGetBufferReadable(xsArg(0), &data, &bytesAvailable))
		data = C_NULL;
	else if (!audioOut->task) {
		xTaskCreate(audioOutLoop, "audioOut", 1536 + XT_STACK_EXTRA_CLIB, audioOut, 10, (TaskHandle_t *)&audioOut->task);
		if (!audioOut->task)
			xsUnknownError("can't create task");
	}
	
	buffer = xsmcToReference(xsArg(0));
	if (xsmcArgc > 1)
		completion = xsmcToReference(xsArg(1));

	element = c_calloc(1, sizeof(AudioOutElementRecord));
	if (!element)
		xsUnknownError("no memory");
	
	element->completion = completion;
	element->buffer = buffer;
	element->data = data;
	element->bytesAvailable = bytesAvailable;

//@@ lock around queue manipulation

	if (audioOut->elements) {
		AudioOutElement walker = audioOut->elements;
		while (walker->next)
			walker = walker->next;
		walker->next = element;
	}
	else
		audioOut->elements = element;

//@@ write to start playing if there's space?
}

void xs_audioout_get_format_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	builtinGetFormat(the, kIOFormatBuffer);
}
 
void xs_audioout_set_format_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	if (kIOFormatBuffer != builtinSetFormat(the))
		xsRangeError("invalid format");
}
 
void xs_audioout_get_bitsPerSample_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	xsmcSetInteger(xsResult, audioOut->bitsPerSample);
}
 
void xs_audioout_get_numChannels_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	xsmcSetInteger(xsResult, audioOut->numChannels);
}
 
void xs_audioout_get_sampleRate_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	xsmcSetInteger(xsResult, audioOut->sampleRate);
}
 
void xs_audioout_get_volume_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	xsmcSetNumber(xsResult, audioOut->volumeDouble);
}
 
void xs_audioout_set_volume_(xsMachine *the)
{
	AudioOut audioOut = xsmcGetHostDataValidate(xsThis, (void *)&xsAudioOutHooks);
	double volume = xsmcToNumber(xsArg(0));
	if ((volume < 0) || (volume > 1))
		xsRangeError("invalid volume");
	audioOut->volumeDouble = volume;
	audioOut->volumeFixed = (int16_t)(volume * 256);
}
 
void xs_audioout_mark_(xsMachine* the, void* it, xsMarkRoot markRoot)
{
	AudioOut audioOut = it;
	AudioOutElement walker;

	if (audioOut->onWritable)
		(*markRoot)(the, audioOut->onWritable);

	for (walker = audioOut->elements; C_NULL != walker; walker = walker->next) {
		if (walker->buffer)
			(*markRoot)(the, walker->buffer);
		if (walker->completion)
			(*markRoot)(the, walker->completion);
	}
}

static bool playedBuffer(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
{
	AudioOut audioOut = user_ctx;

//@@ entire operation on audioOut->bytesWritable should be atomic
	uint32_t bytesWritable = __atomic_add_fetch(&audioOut->bytesWritable, event->size, __ATOMIC_SEQ_CST);
	if (bytesWritable > audioOut->total_dma_buf_size)
		audioOut->bytesWritable = audioOut->total_dma_buf_size;

	if (audioOut->elements) {
		AudioOutElement walker;
		uint32_t samplesAvailable = 0;
		for (walker = audioOut->elements; NULL != walker; walker = walker->next) {
			if (!walker->bytesAvailable)		// buffer already used
				continue;
			if (C_NULL == walker->data)			// relocatable buffer
				break;
			samplesAvailable += walker->bytesAvailable;
			if (samplesAvailable >= audioOut->dma_buf_size) {
				xTaskNotify(audioOut->task, 1, eSetValueWithOverwrite);	// wake task
				return false;		// won't invoke audiooutDeliver / onWritable 
			}
		}
	}

	if (!audioOut->callbackPending) {
audioOut->useCount += 1;		//@@ atomic
		audioOut->callbackPending = true;
		modMessagePostToMachine(audioOut->the, C_NULL, 0, audiooutDeliver, audioOut);		
	}
    return false;
}

static void audiooutDeliver(void *theIn, void *refcon, uint8_t *message, uint16_t messageLength)
{
	xsMachine *the = (xsMachine *)theIn;
	AudioOut audioOut = (AudioOut)refcon;

	if (1 == audioOut->useCount)
		goto done;

	audioOut->callbackPending = false;
	xsBeginHost(the);

	if (audioOut->bytesWritable) {
		AudioOutElement element;
		if (audioOut->onWritable) {
			uint32_t bytesWritable = audioOut->bytesWritable;
			
			for (element = audioOut->elements; C_NULL != element; element = element->next) {
				if (element->bytesAvailable >= bytesWritable) {
					bytesWritable = 0;
					break;
				}
				bytesWritable -= element->bytesAvailable;
			}

			if (bytesWritable) {
				xsTry {
					xsmcSetInteger(xsResult, audioOut->bytesWritable);
					xsCallFunction1(xsReference(audioOut->onWritable), audioOut->obj, xsResult);
				}
				xsCatch {
				}
				if (audioOut->useCount == 0) goto abort;
			}
		}
		
		if (audioOut->bytesWritable && audioOut->elements) {
			for (element = audioOut->elements; (C_NULL != element) && audioOut->bytesWritable; element = element->next) {
				int use = audioOut->bytesWritable;
				if (use > element->bytesAvailable)
					use = element->bytesAvailable;
				if (!use)
					continue;

				void *data = element->data;
				if (!data) {
					xsUnsignedValue currentSize;
					xsResult = xsReference(element->buffer);
					xsmcGetBufferReadable(xsResult, &data, &currentSize);
					
					if ((element->position + use) > currentSize) {		// buffer resized to smaller than original request
						element->err = 1;
						element->bytesAvailable = 0;
						continue;
					}
				}

				element->err = doWrite(audioOut, element->position + (uint8_t *)data, use);
				element->position += use;
				element->bytesAvailable -= use;
				if (element->err)
					element->bytesAvailable = 0;
			}
		}
	}

	while (audioOut->elements && (0 == audioOut->elements->bytesAvailable)) { 
		AudioOutElement element = audioOut->elements;
		audioOut->elements = element->next;

		if (element->completion) {
			xsTry {
				if (element->err) {
					xsmcSetInteger(xsResult, element->err);
					xsResult = xsNew1(xsGlobal, xsID_Error, xsResult);
				}
				else
					xsmcSetNull(xsResult);
				xsCallFunction1(xsReference(element->completion), audioOut->obj, xsResult);
			}
			xsCatch {
			}
		}
		c_free(element);
		if (audioOut->useCount == 0) goto abort;
	}

abort:
	xsEndHost(the);
	
done:
#if defined(_NO_ATOMICS)
	if (0 == --audioOut->useCount)
#else
	if (0 == __atomic_sub_fetch(&audioOut->useCount, 1, __ATOMIC_SEQ_CST))
#endif
	{
		c_free(audioOut);
		return;
	}

}

esp_err_t doWrite(AudioOut audioOut, void *buffer, xsUnsignedValue requested)
{
	esp_err_t err;
	size_t bytes_written;

	const int kTimeout = 100;	//@@ why does this need to be so big? 0 would be nice....
	if (256 == audioOut->volumeFixed) {
		if (audioOut->started)
			err = i2s_channel_write(audioOut->tx_handle, (const char *)buffer, requested, &bytes_written, kTimeout);
		else
			err = i2s_channel_preload_data(audioOut->tx_handle, (const char *)buffer, requested, &bytes_written);
	}
	else {
		int16_t *src = (int16_t *)buffer;
		int16_t volumeFixed = audioOut->volumeFixed;
		int requestedSamples = requested >> 1;		//@@ broken for stereo & 8 bit
		while (requestedSamples) {
			const int samplesLength = 256;
			int16_t samples[samplesLength];
			int use = (samplesLength > requestedSamples) ? requestedSamples : samplesLength, i;
			for (i = 0; i < use; i++)
				samples[i] = (*src++ * volumeFixed) >> 8;

			if (audioOut->started)
				err = i2s_channel_write(audioOut->tx_handle, (const char *)samples, use * 2, &bytes_written, kTimeout);
			else
				err = i2s_channel_preload_data(audioOut->tx_handle, (const char *)samples, use * 2, &bytes_written);
			if (err) break;

			requestedSamples -= use;
		}
	}

	if (err)
		modLog("write err");
	else
		__atomic_fetch_sub(&audioOut->bytesWritable, requested, __ATOMIC_SEQ_CST);
	
	return err;
}

void audioOutLoop(void *pvParameter)
{
	AudioOut audioOut = (AudioOut)pvParameter;

	while (true) {
		uint32_t newState;
		uint8_t doCallback = 0;
		AudioOutElement element;

		xTaskNotifyWait(0, 0, &newState, portMAX_DELAY);
		if (2 == newState)
			break;

		for (element = audioOut->elements; (C_NULL != element) && audioOut->bytesWritable; element = element->next) {
			int use = audioOut->bytesWritable;
			if (use > element->bytesAvailable)
				use = element->bytesAvailable;
			if (!use)
				continue;

			void *data = element->data;
			if (C_NULL == data) {		// cannot access relocatable data from here. should never get here.... because playedBuffer checks 
				doCallback = 1;
				break;
			}

			element->err = doWrite(audioOut, element->position + (uint8_t *)data, use);
			if (element->err)
				element->bytesAvailable = 0;
			else
				element->bytesAvailable -= use;
			element->position += use;

			if (0 == element->bytesAvailable) {
				element->buffer = C_NULL;
				doCallback = 1;
			}
		}

		if (doCallback && !audioOut->callbackPending) {
			audioOut->callbackPending = true;
			audioOut->useCount += 1;		//@@ atomic
			modMessagePostToMachine(audioOut->the, C_NULL, 0, audiooutDeliver, audioOut);
		}
	}

	audioOut->task = NULL;

	vTaskDelete(NULL);	// "If it is necessary for a task to exit then have the task call vTaskDelete( NULL ) to ensure its exit is clean."
}
