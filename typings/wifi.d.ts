/*
* Copyright (c) 2019-2024 Bradley Farias
*
*   This file is part of the Moddable SDK Tools.
*
*   The Moddable SDK Tools is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   The Moddable SDK Tools is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
*
*/

declare module "wifi" {
  export type WiFiOptions = {
    bssid?: ArrayBuffer,
    ssid: string,
    password?: string
  }
  export type WiFiCallback = (message: "connect" | "gotIP" | "lostIP" | "disconnect" | "station_connect" | "station_disconnect") => void
  export type WiFiScanCallback = (item: {
    ssid: string,
    authentication: string,
    rssi: number,
    bssid: ArrayBuffer,
  } | null) => void;
  export type WiFiMode = -5 | 0 | 1 | 2 | 3;
  export type ScanOptions = {hidden?: boolean, channel?: number}
  export type AccessPointOptions = {
    ssid: string,
    password?: string,
    channel?: number,
    hidden?: boolean,
    interval?: number,
    max?: number
  }

  class WiFi {
    static readonly gotIP: "gotIP";
    static readonly lostIP: "lostIP";
    static readonly connected: "connect";
    static readonly disconnected: "disconnect";
    static readonly station_connected: "station_connect";
    static readonly station_disconnected: "station_disconnect";
    static readonly Mode: {
      off: -5,
      none: 0,
      station: 1,
      accessPoint: 2,
    };

    constructor(options?: WiFiOptions, callback?: WiFiCallback);
    close(): void;
    static scan(options: ScanOptions, callback: WiFiScanCallback): void;
    static mode: WiFiMode;
    static connect(options?: WiFiOptions): void;
    static disconnect(): void;
    static accessPoint(options: AccessPointOptions): void;
  }
  export {WiFi as default};
}