import { LoginSDK } from '../sdk/SDKManager'
import { PaySDK } from '../sdk/SDKManager'

export class SDKNapiHelper {

    static registerFunctions(registerFunc : Function) {
        SDKNapiHelper.registerLoginSDK(registerFunc);
        SDKNapiHelper.registerPaySDK(registerFunc);
    }

    private static registerLoginSDK(registerFunc : Function) {

      registerFunc('LoginSDK.loginInit', LoginSDK.loginInit);
      registerFunc('LoginSDK.on', LoginSDK.on);
      registerFunc('LoginSDK.unionLogin', LoginSDK.unionLogin);
      registerFunc('LoginSDK.authorization', LoginSDK.authorization);
      registerFunc('LoginSDK.playerInfoVerification', LoginSDK.playerInfoVerification);
      registerFunc('LoginSDK.bindPlayer', LoginSDK.bindPlayer);
      registerFunc('LoginSDK.verifyLocalPlayer', LoginSDK.verifyLocalPlayer);
      registerFunc('LoginSDK.savePlayerRole', LoginSDK.savePlayerRole);
      registerFunc('LoginSDK.authorization2', LoginSDK.authorization2);
      registerFunc('LoginSDK.verifyLocalPlayer2', LoginSDK.verifyLocalPlayer2);
      registerFunc('LoginSDK.savePlayerRole2', LoginSDK.savePlayerRole2);

    }
    private static registerPaySDK(registerFunc : Function) {
      registerFunc('PaySDK.checkPayEnv', PaySDK.checkPayEnv);
      registerFunc('PaySDK.queryConsumProdts', PaySDK.queryConsumProdts);
      registerFunc('PaySDK.buy', PaySDK.buy);
      registerFunc('PaySDK.queryNoConsumableGoods', PaySDK.queryNoConsumableGoods);
      registerFunc('PaySDK.purchaseOfNonConsumableGoods', PaySDK.purchaseOfNonConsumableGoods);
      registerFunc('PaySDK.disposableGoodsOrderQuery', PaySDK.disposableGoodsOrderQuery);
      registerFunc('PaySDK.nonConsumableProductsRecords', PaySDK.nonConsumableProductsRecords);
      registerFunc('PaySDK.consumableProductsRecords', PaySDK.consumableProductsRecords);
      registerFunc('PaySDK.noConsumGoodsService', PaySDK.noConsumGoodsService);
      registerFunc('PaySDK.queryAutorenewGoods', PaySDK.queryAutorenewGoods);
      registerFunc('PaySDK.purchaseOfAutoRenewGoods', PaySDK.purchaseOfAutoRenewGoods);
      registerFunc('PaySDK.autoRenewProductsRecords', PaySDK.autoRenewProductsRecords);
      registerFunc('PaySDK.autoRenewGoodsService', PaySDK.autoRenewGoodsService);
    }
}