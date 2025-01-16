import Logger from '../utils/Logger'

let log: Logger = new Logger(0x0001, "SDKManager");

export class LoginSDK {
  static MODULE_NAME: string = 'LoginSDK';
  private static parentPort;

  static init(parentPort) {
    LoginSDK.parentPort = parentPort;
  }

  static loginInit(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'loginInit', 'viewTag': "1111"
    });
    return "";
  }
  static on(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'on', 'viewTag': "1111"
    });
    return "";
  }
  static unionLogin(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'unionLogin', 'viewTag': "1111"
    });
    return "";
  }
  static authorization(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'authorization', 'viewTag': "1111"
    });
    return "";
  }
  static playerInfoVerification(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'playerInfoVerification', 'viewTag': "1111"
    });
    return "";
  }
  static bindPlayer(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'bindPlayer', 'viewTag': "1111"
    });
    return "";
  }
  static verifyLocalPlayer(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'verifyLocalPlayer', 'viewTag': "1111"
    });
    return "";
  }
  static savePlayerRole(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'savePlayerRole', 'viewTag': "1111"
    });
    return "";
  }
  static authorization2(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'authorization2', 'viewTag': "1111"
    });
    return "";
  }
  static verifyLocalPlayer2(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'verifyLocalPlayer2', 'viewTag': "1111"
    });
    return "";
  }
  static savePlayerRole2(): string {
    LoginSDK.parentPort.postMessage({
      'module': LoginSDK.MODULE_NAME, 'function': 'savePlayerRole2', 'viewTag': "1111"
    });
    return "";
  }
}
export class PaySDK {
  static MODULE_NAME: string = 'PaySDK';
  private static parentPort;

  static init(parentPort) {
    PaySDK.parentPort = parentPort;
  }

  static checkPayEnv(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'checkPayEnv', 'viewTag': "1111"
    });
    return "";
  }
  static queryConsumProdts(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'queryConsumProdts', 'viewTag': "1111"
    });
    return "";
  }
  static buy(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'buy', 'viewTag': "1111"
    });
    return "";
  }
  static queryNoConsumableGoods(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'queryNoConsumableGoods', 'viewTag': "1111"
    });
    return "";
  }
  static purchaseOfNonConsumableGoods(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'purchaseOfNonConsumableGoods', 'viewTag': "1111"
    });
    return "";
  }
  static disposableGoodsOrderQuery(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'disposableGoodsOrderQuery', 'viewTag': "1111"
    });
    return "";
  }
  static nonConsumableProductsRecords(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'nonConsumableProductsRecords', 'viewTag': "1111"
    });
    return "";
  }
  static consumableProductsRecords(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'consumableProductsRecords', 'viewTag': "1111"
    });
    return "";
  }
  static noConsumGoodsService(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'noConsumGoodsService', 'viewTag': "1111"
    });
    return "";
  }
  static queryAutorenewGoods(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'queryAutorenewGoods', 'viewTag': "1111"
    });
    return "";
  }
  static purchaseOfAutoRenewGoods(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'purchaseOfAutoRenewGoods', 'viewTag': "1111"
    });
    return "";
  }
  static autoRenewProductsRecords(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'autoRenewProductsRecords', 'viewTag': "1111"
    });
    return "";
  }
  static autoRenewGoodsService(): string {
    PaySDK.parentPort.postMessage({
      'module': PaySDK.MODULE_NAME, 'function': 'autoRenewGoodsService', 'viewTag': "1111"
    });
    return "";
  }
}