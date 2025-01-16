import cryptoFramework from '@ohos.security.cryptoFramework';
import { GlobalContext, GlobalContextConstants } from '../common/GlobalContext';

import { authentication } from '@kit.AccountKit';
import { gamePlayer } from '@kit.GameServiceKit';
import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { iap } from '@kit.IAPKit';
import { JWTUtil } from '../common/JWTUtil';
import { Logger } from '../common/Logger';
import { Callback, BusinessError } from '@kit.BasicServicesKit';
import { ArrayList, util } from '@kit.ArkTS';

export interface EventData {
  'module': string,
  'function': string,
  'viewTag': string
}

interface PurchaseOrderPayload {
  productType: number;
  purchaseToken: string;
  purchaseOrderId: string;
}


class SDKManagerMsg {

  authorizationCode:string = "";
  unionLoginResult:gamePlayer.UnionLoginResult = {
    accountName: '',
    needBinding: false,
    localPlayer: undefined,
    boundPlayerInfo: undefined
  }  ;

  thirdAccountInfo1: gamePlayer.ThirdAccountInfo = {
    'accountName': 'testName1',
    'accountIcon': $r('app.media.icon')
  };
  thirdAccountInfo2: gamePlayer.ThirdAccountInfo = {
    'accountName': 'testName2',
    'accountIcon': $r('app.media.icon')
  };
  thirdAccountInfos:Array<gamePlayer.ThirdAccountInfo> =  [
    this.thirdAccountInfo1,
    this.thirdAccountInfo2
  ]

  async handlePullSDK(eventData: EventData): Promise<string> {
    let context: common.UIAbilityContext = GlobalContext.loadGlobalThis(GlobalContextConstants.COCOS2DX_ABILITY_CONTEXT);
    switch (eventData.function) {
    /**
     * 初始化登录
     * Game Service Kit 向玩家弹出华为隐私协议弹框要求用户签署隐私协议。
     − 如果玩家此时不签署协议，则会退出当前游戏。
     − 如果玩家已经签署隐私协议，则不会再弹出隐私协议弹框，并继续执行步骤下一步
     */
      case "loginInit":
        return await this.handleLoginInit(context);
    //初始化登录成功后，游戏调用on接口注册playerChanged事件监听
      case "on":
        return await this.on(context);
    //游戏调用 unionLogin 接口进行联合登录
      case "unionLogin":
        return await this.unionLogin(context);
    // Game Service Kit 向玩家展示联合登录弹框
    /******************联合登录场景一：玩家选择“华为账号快速登录”方式*******/
      case "authorization":
        return await this.handleAuthorization(context);
    // 玩家信息校验
      case "playerInfoVerification":
        return await this.playerInfoVerification(context);
    // 绑定
      case "bindPlayer":
        return await this.bindPlayer(context);
    // 合规校验
      case "verifyLocalPlayer":
        return await this.verifyLocalPlayer(context);
    // 玩家角色信息上报
      case "savePlayerRole":
        return await this.savePlayerRole(context);
    /******************联合登录场景二：玩家使用“游戏官方账号登录”方式*******/
    // 游戏判断 accountName 为 unionLogin 接口传入的 thirdAccountInfo.accountName 或 “official_account”时，进行游戏官方账号自行登录处理
      case "authorization2":
        return await this.handleAuthorization(context);
    // 玩家选择游戏官方账号成功登录后，游戏调用 verifyLocalPlayer 接口进行合规校验，华为侧将校验当前设备的账号实名认证和游戏防沉迷管控情况，如校验未通过将返回对应的错误码。
      case "verifyLocalPlayer2":
        return await this.verifyLocalPlayer(context);
    // 合规校验成功后，在玩家创建角色时，游戏必须调用 savePlayerRole 将玩家角色信息上报。
      case "savePlayerRole2":
        return await this.savePlayerRole(context);
    //检查支付环境
      case "checkPayEnv":
        return await this.handleCheckPayEnv(context);
    //查询消耗商品
      case "queryConsumProdts":
        return await this.handleQueryConsumableGoods(context);
    //查询非消耗型商品
      case "queryNoConsumableGoods":
        return await this.handleQueryNoConsumableGoods(context);
    //查询订阅型商品
      case "queryAutorenewGoods":
        return await this.handleQueryAutoRenewGoods(context);
    //消耗型商品购买
      case "buy":
        return await this.handlePurchaseOfConsumableGoods(context);
    //非消耗型商品购买
      case "purchaseOfNonConsumableGoods":
        return await this.handlePurchaseOfNonConsumableGoods(context);
    //订阅型商品购买
      case "purchaseOfAutoRenewGoods":
        return await this.handlePurchaseOfAutoRenewGoods(context);
    //查询非消耗型商品的购买记录
      case "nonConsumableProductsRecords":
        return await this.handleNonConsumableProductsRecords(context);
    //查询消耗型商品的购买记录
      case "consumableProductsRecords":
        return await this.handleConsumableProductsRecords(context);
    //查询订阅型商品的购买记录
      case "autoRenewProductsRecords":
        return await this.handleAutoRenewProductsRecords(context);
    //消耗型商品掉单查询
      case "disposableGoodsOrderQuery":
        return await this.handleDisposableGoodsOrderQuery(context);
    //非消耗型商品提供服务
      case "noConsumGoodsService":
        return await this.handleNoConsumGoodsService(context);
    //订阅型商品提供服务
      case "autoRenewGoodsService":
        return await this.handleAutoRenewGoodsService(context);
      default: {
        return Promise.resolve("");
      }
    }
  }

  //初始化登录
  async handleLoginInit(context: common.UIAbilityContext): Promise<string> {
    try {
      await gamePlayer.init(context as common.UIAbilityContext);
      console.log('init success.');
      return 'init success!';
    } catch (error) {
      // 这里处理异步的参数错误和业务逻辑执行错误。
      hilog.error(0x0001, 'Login Demo', `init failed: Error: ${error}`);
      return `init failed: Error: ${JSON.stringify(error)}`;
    }
  }

  //初始化登录成功后，游戏调用on接口注册playerChanged事件监听
  async on(context: common.UIAbilityContext): Promise<string> {
    let callback: Callback<gamePlayer.PlayerChangedResult> = ((result) => {
      if (result.event === gamePlayer.PlayerChangedEvent.SWITCH_GAME_ACCOUNT) {
        // 游戏号已切换，完成本地缓存清理工作后，再次调用 unionLogin 接口等
        this.unionLoginResult = {
          accountName: '',
          needBinding: false,
          localPlayer: undefined,
          boundPlayerInfo: undefined
        };
        this.unionLogin(context);
      }
    });
    try {
      await gamePlayer.on('playerChanged', callback);
      return 'GamePlayer on success!';
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `GamePlayer on registered exception: ${error}`);
      return `on failed: Error: ${JSON.stringify(error)}`;
    };

  }

  //游戏调用 unionLogin 接口进行联合登录
  async unionLogin(context: common.UIAbilityContext): Promise<string> {
    let request: gamePlayer.UnionLoginParam = {
      showLoginDialog: true,
      thirdAccountInfos: this.thirdAccountInfos
    };
    try {
      let result:gamePlayer.UnionLoginResult =  await gamePlayer.unionLogin(context, request);
      this.unionLoginResult = result;
      return result.accountName;
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `unionLogin exception: ${error}`);
      return `unionLogin failed: Error: ${JSON.stringify(error)}`;
    }
  }

  // 此处在云侧完成
  // 玩家信息校验 如果您没有自己的服务器，可以在客户端完成玩家标识信息校验。但为了保证安全性，强烈建议您在服务器侧验证玩家标识。
  async playerInfoVerification(context: common.UIAbilityContext): Promise<string> {
    // 游戏客户端将玩家信息（Authorization Code、玩家标识等）上传给游戏开发 者服务器。

    // 开发者服务器调用华为账号服务器的获取凭证 Access Token 接口，根据Authorization Code 获取玩家的 Access Token。

    // 开发者服务器调用华为游戏服务器的获取玩家标识接口，根据 Access Token 获取服务器侧的玩家标识。

    // 与 unionLogin 接口获取的玩家标识进行比对，完成玩家信息的校验。玩家标识校验通过后，您可以允许玩家进入游戏。

    return "This is done on the cloud side.";

  }

  // 当 needBinding 值为 true 时，需先判定获取的手机号以及玩家标识信息（与游戏官方账号）的注册与绑定情况，然后将玩家标识与游戏官方账号进行绑定。
  async bindPlayer(context: common.UIAbilityContext): Promise<string> {
    // 判断授权获取的手机号是否已注册游戏官方账号。如未注册，则先进行注册。

    // 判断 teamPlayerId 与游戏官方账号是否已绑定。如未绑定，则进行绑定。如已绑定，则继续执行。

    // 游戏调用 bindPlayer 接口绑定并上报绑定结果。
    if(this.unionLoginResult.needBinding==true) {
      let thirdOpenId = '123xxxx'; // thirdOpenId 表示游戏官方账号 ID
      let teamPlayerId = '456xxx'; // teamPlayerId 表示玩家华为账号对应的 teamPlayerId
      try {
        await gamePlayer.bindPlayer(context, thirdOpenId, teamPlayerId);
        return 'bindPlayer success.';
      } catch (error) {
        hilog.error(0x0000, 'Login Demo', `bindPlayer failed: ${error}`);
        return 'bindPlayer failed:' + JSON.stringify(error);
      };
    }
  }
  // 游戏调用 verifyLocalPlayer接口进行合规校验，华为侧将校验当前设备的账号实名 认证和游戏防沉迷管控情况，如校验未通过将返回对应的错误码
  async verifyLocalPlayer(context: common.UIAbilityContext): Promise<string> {
    let request: gamePlayer.ThirdUserInfo = {
      thirdOpenId: '123xxxx', // 游戏官方账号 ID
      isRealName: true, // 玩家是否实名,该值为 true 时表示已实名,为 false 时表示未实名
    };
    try {
      await gamePlayer.verifyLocalPlayer(context, request);
      return 'verifyLocalPlayer success.';
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `verifyLocalPlayer exception: ${error}`);
      return 'verifyLocalPlayer failed:' + JSON.stringify(error);
    };
  }

  // 合规校验成功后，在玩家创建角色时，游戏必须调用 savePlayerRole 将玩家角色信息上报。
  async savePlayerRole(context: common.UIAbilityContext): Promise<string> {
    // 注意：请在用户登录并选择角色以及区服后调用。如果游戏没有角色系统，roleId 请传入“0”，roleName 请传入“default”
    let request: gamePlayer.GSKPlayerRole = {
      roleId: '123', //玩家角色 ID，如游戏没有角色系统，请传入“0”，务必不要传""和 null。
      roleName: 'Jason', //玩家角色名，如游戏没有角色系统，请传入“default”，务必不要传""和 null。
      serverId: '456',
      serverName: 'Zhangshan',
      gamePlayerId: '789', //根据实际获取到的 gamePlayerId 传值。
    };
    try {
      await gamePlayer.savePlayerRole(context, request);
      return 'savePlayerRole success.';
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `savePlayerRole exception: ${error}`);
      return 'savePlayerRole failed:' + JSON.stringify(error);
    }
  }

  async authorizationNeedBinding(context: common.UIAbilityContext) {
    // 1. 创建授权请求，并设置参数
    let authRequest = await new authentication.HuaweiIDProvider().createAuthorizationWithHuaweiIDRequest();
    // 获取头像昵称需要传如下 scope；如需授权获取用户手机号，应先完成“获取您的手机号”的 scope 权限申请，并在 authRequest.scopes 中传入"phone"。如 scope 审批未完成或未通过，将无法正常展示和获取用户授权手机号
    authRequest.scopes = ['profile'];
    // 若开发者需要进行服务端开发，则需传如下 permission 获取 authorizationCode
    authRequest.permissions = ['serviceauthcode'];
    // 用户是否需要登录授权，该值为 true 且用户未登录或未授权时，会拉起用户登录或授权页面
    authRequest.forceAuthorization = true;
    authRequest.state = util.generateRandomUUID();
    // 2. 调用 AuthenticationController 对象的 executeRequest 方法执行授权请求，并在 Callback 中处理授权结果，从授权结果中解析出头像昵称。
    try {
      // 执行授权请求
      let controller = new authentication.AuthenticationController(context);
      controller.executeRequest(authRequest, (err, data) => {
        if (err) {
          hilog.error(0x0000, 'Login Demo', `auth failed,error: ${err}`);
          return 'handleAuthorization failed:' + JSON.stringify(err);
        }
        let authorizationWithHuaweiIDResponse = data as authentication.AuthorizationWithHuaweiIDResponse;
        let state = authorizationWithHuaweiIDResponse.state;
        if (state != undefined && authRequest.state != state) {
          hilog.error(0x0000, 'Login Demo', `auth failed, state is different: ${authorizationWithHuaweiIDResponse}`);
          return 'login failed, state is different';
        }
        hilog.debug(0x0000, 'Login Demo', `auth succeeded: ${authorizationWithHuaweiIDResponse}`);
        let authorizationWithHuaweiIDCredential = authorizationWithHuaweiIDResponse.data!;
        let avatarUri = authorizationWithHuaweiIDCredential.avatarUri;
        let nickName = authorizationWithHuaweiIDCredential.nickName;
        this.authorizationCode= authorizationWithHuaweiIDCredential.authorizationCode;
        // 开发者处理 vatarUri, nickName, authorizationCode 信息

      });
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `auth failed: ${error}`);
      this.authorizationCode= ""
    }
  }

  async authorizationNoNeedBinding(context: common.UIAbilityContext) {
    //1. 调用 createLoginWithHuaweiIDRequest 创建登录请求并设置参数。
    // 创建登录请求，并设置参数
    let loginRequest = await new authentication.HuaweiIDProvider().createLoginWithHuaweiIDRequest();
    // 当用户未登录华为账号时，是否强制拉起华为账号登录界面
    loginRequest.forceLogin = true;
    loginRequest.state = util.generateRandomUUID();
    //2. 调用 AuthenticationController 对象的 executeRequest 方法执行登录请求，并在 Callback 中处理登录结果，获取到 Authorization Code。
    try {
      // 执行登录请求
      let controller = new authentication.AuthenticationController(context);
      controller.executeRequest(loginRequest, (err, data) => {
        if (err) {
          hilog.error(0x0000, 'Login Demo', `login failed, error: ${err}`);
          return 'handleAuthorization failed:' + JSON.stringify(err);
        }
        let loginWithHuaweiIDResponse = data as authentication.LoginWithHuaweiIDResponse;
        let state = loginWithHuaweiIDResponse.state;
        if (state != undefined && loginRequest.state != state) {
          hilog.error(0x0000, 'Login Demo', `login failed, state is different: ${loginWithHuaweiIDResponse}`);
          return 'login failed, state is different';
        }
        hilog.debug(0x0000, 'Login Demo', `login succeeded: ${loginWithHuaweiIDResponse}`);
        let loginWithHuaweiIDCredential = loginWithHuaweiIDResponse.data!;
        this.authorizationCode=  loginWithHuaweiIDCredential.authorizationCode;
        // 开发者处理 authorizationCode
      });
    } catch (error) {
      hilog.error(0x0000, 'Login Demo', `login failed: ${error}`);
      this.authorizationCode= ""
    }
  }

  //华为账号授权
  async handleAuthorization(context: common.UIAbilityContext): Promise<string> {
    //玩家成功登录后，游戏获取到玩家标识、needBinding、accountName 等信息(UnionLoginResult)
    //游戏判断 accountName 为"hw_account"时，通过 Account Kit 对应的创建授权/登录请求接口，获取用于服务器校验的 Authorization Code 信息
    if(this.unionLoginResult.accountName =='hw_account'){
      if(this.unionLoginResult.needBinding==true){
        // 当 needBinding 为 true 时（关联场景），华为玩家标识需与游戏官方账号进行关联绑定，应调用 createAuthorizationWithHuaweiIDRequest 接口创建授权请求（授权获取手机号）。
        this.authorizationNeedBinding(context);
        return "authorization need binding success!";
      }else{
        // − 当 needBinding 为 false 时（转移场景），不需要华为玩家标识与游戏官方账号绑定，调用createLoginWithHuaweiIDRequest 接口创建登录请求。
        this.authorizationNoNeedBinding(context);
        return "authorization no need binding  success!";
      }
    }else {
      let accountNames:ArrayList<String> =new ArrayList<String>();
      accountNames.add("official_account");
      this.thirdAccountInfos.forEach(thirdAccountInfo => {
        accountNames.add(thirdAccountInfo.accountName);
      });
      if (accountNames.has(this.unionLoginResult.accountName)){
        // 游戏判断 accountName 为 unionLogin 接口传入的 thirdAccountInfo.accountName 或“official_account”时，进行游戏官方账号自行登录处理
        return this.unionLoginResult.accountName;
      }
    }
  }
  //检查支付环境
  async handleCheckPayEnv(context: common.UIAbilityContext): Promise<string> {
    try {
      await iap.queryEnvironmentStatus(context);
     return 'check pay env success!';
    } catch (error) {
      // 这里处理异步的参数错误和业务逻辑执行错误。
      hilog.error(0x0001, "Iap Demo", `Iap queryEnvironmentStatus: Error: ${JSON.stringify(error)}`);
       return `check pay env failed: Error: ${JSON.stringify(error)}`;
    }
  }

  //查询消耗型商品
  async handleQueryConsumableGoods(context: common.UIAbilityContext): Promise<string> {
    let queryProductParam: iap.QueryProductsParameter = {
      // iap.ProductType.CONSUMABLE：消耗型商品;
      // iap.ProductType.NONCONSUMABLE：非消耗型商品;
      productType: iap.ProductType.CONSUMABLE,
      // 查询的商品必须是您在AppGallery Connect网站配置的商品
      productIds: ['consumeProduct001']
    };
    return await this.queryProducts(context, queryProductParam);
  }

  //查询非消耗型商品
  async handleQueryNoConsumableGoods(context: common.UIAbilityContext): Promise<string> {
    let queryProductParam: iap.QueryProductsParameter = {
      // iap.ProductType.CONSUMABLE：消耗型商品;
      // iap.ProductType.NONCONSUMABLE：非消耗型商品;
      productType: iap.ProductType.NONCONSUMABLE,
      // 查询的商品必须是您在AppGallery Connect网站配置的商品
      productIds: ['noConsumeProduct001']
    };
    return await this.queryProducts(context, queryProductParam);
  }

  //查询订阅型商品
  async handleQueryAutoRenewGoods(context: common.UIAbilityContext): Promise<string> {
    let queryProductParam: iap.QueryProductsParameter = {
      productType: iap.ProductType.AUTORENEWABLE,
      // 查询的商品必须是您在AppGallery Connect网站配置的商品
      productIds: ['subscribeProduct001']
    };
    return await this.queryProducts(context, queryProductParam);
  }

  async queryProducts(context: common.UIAbilityContext, queryProductParam:iap.QueryProductsParameter ): Promise<string> {
    try {
      // 获取成功
      let products = await iap.queryProducts(context, queryProductParam);
      // 处理成功逻辑
      hilog.info(0x0001, 'Iap Demo', `Iap queryProducts: data:` + JSON.stringify(products));
      return 'queryProducts: data: ' + JSON.stringify(products);
    } catch (err) {
      hilog.error(0x0001, "Iap Demo", `Iap queryProducts: Error: ${JSON.stringify(err)}`);
      return `Iap queryProducts: Error: ${JSON.stringify(err)}`;
    }
  }
  //消耗型商品购买
  async handlePurchaseOfConsumableGoods(context: common.UIAbilityContext): Promise<string> {
    //购买
    const createPurchaseParam: iap.PurchaseParameter = {
      // 通过purchase接口购买的商品必须是您在AppGallery Connect网站配置的商品
      productId: 'consumeProduct001',
      // iap.ProductType.CONSUMABLE：消耗型商品;
      // iap.ProductType.NONCONSUMABLE：非消耗型商品;
      productType: iap.ProductType.CONSUMABLE
    }
    return await this.purchase(context, createPurchaseParam);
  }
  //非消耗型商品购买
  async handlePurchaseOfNonConsumableGoods(context: common.UIAbilityContext): Promise<string> {
    //购买
    const createPurchaseParam: iap.PurchaseParameter = {
      // 通过purchase接口购买的商品必须是您在AppGallery Connect网站配置的商品
      productId: 'noConsumeProduct001',
      // iap.ProductType.CONSUMABLE：消耗型商品;
      // iap.ProductType.NONCONSUMABLE：非消耗型商品;
      productType: iap.ProductType.NONCONSUMABLE
      // developerPayload: 'testPurchase'
    }
    return await this.purchase(context, createPurchaseParam);
  }
  //订阅型商品购买
  async handlePurchaseOfAutoRenewGoods(context: common.UIAbilityContext): Promise<string> {
    //购买
    const createPurchaseParam: iap.PurchaseParameter = {
      // 通过purchase接口购买的商品必须是您在AppGallery Connect网站配置的商品
      productId: 'subscribeProduct001',
      productType: iap.ProductType.AUTORENEWABLE
    }
    return await this.purchase(context, createPurchaseParam);
  }
  async purchase(context: common.UIAbilityContext,createPurchaseParam:iap.PurchaseParameter): Promise<string> {
    try {
      let result = await iap.createPurchase(context, createPurchaseParam);
      hilog.info(0x0001, 'Iap Demo', JSON.stringify(result))

      let jwsPurchaseOrder: string = JSON.parse(result.purchaseData).jwsPurchaseOrder;
      let purchaseStr = JWTUtil.decodeJwtObj(jwsPurchaseOrder);
      let purchaseOrderPayload = JSON.parse(purchaseStr) as PurchaseOrderPayload;
      // 请在权益发放成功后（而不是发放前或发放中）调用finish
      this.finishPurchase(context, purchaseOrderPayload);
      return 'purchase success';
    } catch (err) {
      hilog.error(0x0001, 'Iap Demo', 'purchase err:' + JSON.stringify(err))
      return 'purchase failed:' + JSON.stringify(err, null, 4);
    }
  }
  async finishPurchase(context: common.UIAbilityContext, purchaseOrder: PurchaseOrderPayload) {
    Logger.info('finishPurchase begin');
    let finishPurchaseParam: iap.FinishPurchaseParameter = {
      productType: purchaseOrder.productType,
      purchaseToken: purchaseOrder.purchaseToken,
      purchaseOrderId: purchaseOrder.purchaseOrderId
    };
    try {
      let res = await iap.finishPurchase(context, finishPurchaseParam);
      Logger.info('finishPurchase success');
    } catch (error) {
      Logger.error(`finishPurchase fail:${error.code}, msg:${error.message}`);
    }
  }
  //查询非消耗型商品的购买记录
  async handleNonConsumableProductsRecords(context: common.UIAbilityContext): Promise<string> {
    //查询非消耗型商品的购买记录
    const recordsParameter: iap.QueryPurchasesParameter = {
      productType: iap.ProductType.NONCONSUMABLE,
      queryType: iap.PurchaseQueryType.ALL
    }
    const res = await iap.queryPurchases(context, recordsParameter);
    return 'queryPurchaseRecords: data' + JSON.stringify(res.purchaseDataList);
  }
  //查询消耗型商品的购买记录
  async handleConsumableProductsRecords(context: common.UIAbilityContext): Promise<string> {
    //查询消耗型商品的购买记录
    const recordsParameter: iap.QueryPurchasesParameter = {
      productType: iap.ProductType.CONSUMABLE,
      queryType: iap.PurchaseQueryType.ALL
    }
    const res = await iap.queryPurchases(context, recordsParameter);
    return 'queryPurchaseRecords: data' + JSON.stringify(res.purchaseDataList);
  }
  //查询订阅型商品的购买记录
  async handleAutoRenewProductsRecords(context: common.UIAbilityContext): Promise<string> {
    //查询消耗型商品的购买记录
    const recordsParameter: iap.QueryPurchasesParameter = {
      productType: iap.ProductType.AUTORENEWABLE,
      queryType: iap.PurchaseQueryType.ALL
    }
    const res = await iap.queryPurchases(context, recordsParameter);
    return 'queryPurchaseRecords: data' + JSON.stringify(res.purchaseDataList);
  }

  async queryPurchases(context: common.UIAbilityContext,recordsParameter: iap.QueryPurchasesParameter): Promise<string> {
    try {
      const res = await iap.queryPurchases(context, recordsParameter);

      let purchaseDataList: string[] = res.purchaseDataList;
      if (purchaseDataList === undefined || purchaseDataList.length <= 0) {
        return 'queryPurchaseRecords: data null.';
      }
      for (let i = 0; i < purchaseDataList.length; i++) {
        let purchaseData = purchaseDataList[i];
        let jwsPurchaseOrder: string = JSON.parse(purchaseData).jwsPurchaseOrder;
        let purchaseStr = JWTUtil.decodeJwtObj(jwsPurchaseOrder);
        let purchaseOrderPayload = JSON.parse(purchaseStr) as PurchaseOrderPayload;
        // 请在权益发放成功后（而不是发放前或发放中）调用finish
        this.finishPurchase(context, purchaseOrderPayload);
      }
      // 处理成功逻辑
      hilog.info(0x0001, 'Iap Demo', `Iap queryPurchaseRecords: data:` + JSON.stringify(purchaseDataList));
      // result返回子线程
      return 'queryPurchaseRecords: data' + JSON.stringify(purchaseDataList);
    } catch (error) {
      hilog.error(0x0001, 'Iap Demo', `Iap queryPurchaseRecords: Error: ${error}`);
      return 'queryPurchaseRecords failed:' + JSON.stringify(error, null, 4);
    }
  }
  //消耗型商品补单
  async handleDisposableGoodsOrderQuery(context: common.UIAbilityContext): Promise<string> {
      //查询消耗型商品的购买记录 补单
      const recordsParameter: iap.QueryPurchasesParameter = {
        productType: iap.ProductType.CONSUMABLE,
        queryType: iap.PurchaseQueryType.UNFINISHED
      }
      return await this.queryPurchases(context, recordsParameter);
  }
  //非消耗型商品提供服务
  async handleNoConsumGoodsService(context: common.UIAbilityContext): Promise<string> {
    const recordsParameter: iap.QueryPurchasesParameter = {
      productType: iap.ProductType.NONCONSUMABLE,
      queryType: iap.PurchaseQueryType.UNFINISHED
    }
    return await this.queryPurchases(context, recordsParameter);
  }
  //订阅型商品提供服务
  async handleAutoRenewGoodsService(context: common.UIAbilityContext): Promise<string> {
    const recordsParameter: iap.QueryPurchasesParameter = {
      productType: iap.ProductType.AUTORENEWABLE,
      queryType: iap.PurchaseQueryType.UNFINISHED
    }
    return await this.queryPurchases(context, recordsParameter);
  }
}
export const sdkManagerMsg = new SDKManagerMsg();
