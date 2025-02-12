import Digital from "pins/digital";
import Timer from "timer";

class AW3641 extends Digital {
    #timer;

	constructor(options = {}) {
		super({
			pin: options.pin,
            mode: Digital.Output
		});
	}
    flash(value){
        if (this.#timer) {
                this.#timer.next = value;	// wait until previous setting is complete
                return;
        }

        for(let i=0;i<value;i++){
            super.write(0);
            super.write(1);
        }
	
       let delay = 0;
        if((value>0)&&(value<9)) delay = 220;
        if((value>8)&&(value<17)) delay = 1300;
        this.#timer = Timer.set(() => {
			super.write(0);

			const next = this.#timer.next;
			this.#timer = undefined;
			if (undefined !== next)
				this.flash(next);
        }, delay);
   }
}
AW3641.off=0;
AW3641.Time220ms_Brightness100=1;
AW3641.Time220ms_Brightness90=2;
AW3641.Time220ms_Brightness80=3;
AW3641.Time220ms_Brightness70=4;
AW3641.Time220ms_Brightness60=5;
AW3641.Time220ms_Brightness50=6;
AW3641.Time220ms_Brightness40=7;
AW3641.Time220ms_Brightness30=8;
AW3641.Time1_3s_Brightness100=9;
AW3641.Time1_3s_Brightness90=10;
AW3641.Time1_3s_Brightness80=11;
AW3641.Time1_3s_Brightness70=12;
AW3641.Time1_3s_Brightness60=13;
AW3641.Time1_3s_Brightness50=14;
AW3641.Time1_3s_Brightness40=15;
AW3641.Time1_3s_Brightness30=16;	

Object.freeze(AW3641.prototype);

export default AW3641;