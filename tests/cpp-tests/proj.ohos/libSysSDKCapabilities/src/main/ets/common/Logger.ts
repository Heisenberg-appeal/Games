import { hilog } from '@kit.PerformanceAnalysisKit';

const IAP_LOG_DOMIN = 0x0ffe;

export class Logger {
    static prefix: string = '[IAP_Demo]';

    public static setPrefix(module: string) {
        this.prefix = "[IAP_Demo." + module + "] ";
    }

    public static log(message, ...args: any[]) {
        hilog.info(IAP_LOG_DOMIN, this.prefix, message, args);
    }

    public static info(message, ...args: any[]) {
        hilog.info(IAP_LOG_DOMIN, this.prefix, message, args);
    }

    public static debug(message, ...args: any[]) {
        hilog.debug(IAP_LOG_DOMIN, this.prefix, message, args);
    }

    public static warn(message, ...args: any[]) {
        hilog.warn(IAP_LOG_DOMIN, this.prefix, message, args);
    }

    public static error(message, ...args: any[]) {
        hilog.error(IAP_LOG_DOMIN, this.prefix, message, args);
    }

    public static err(message, ...args: any[]) {
        hilog.error(IAP_LOG_DOMIN, Logger.prefix, message, args);
    }

    public static log4Json(message, ...args: any[]) {
        let args4Json = []
        for (let arg of args) {
            args4Json.push(JSON.stringify(arg))
        }
        hilog.debug(IAP_LOG_DOMIN, this.prefix, message + args4Json)
    }

    public static printJsonObj(obj) {
        let str = JSON.stringify(obj, null, 4);
        for (let i = 0; i < str.length / 512; i++) {
            hilog.debug(IAP_LOG_DOMIN, this.prefix, str.slice(i * 512, (i + 1) * 512));
        }
    }
}
