#ifndef __CC_APPLICATION_OHOS_SDK_H__
#define __CC_APPLICATION_OHOS_SDK_H__
#include <string>
#include "CCSDKPlatformDefine.h"
namespace cocos2d {
    typedef void (*MsgCallback)(const char *result);
    typedef void (*Callback)(bool res, const char *result);
    class CC_DLL SDKApplication
    {
    public:
        SDKApplication();
        virtual ~SDKApplication();
        static SDKApplication *getInstance();

        static std::string loginInit();
        static std::string on();
        static std::string unionLogin();
        static std::string authorization();
        static std::string playerInfoVerification();
        static std::string bindPlayer();
        static std::string verifyLocalPlayer();
        static std::string savePlayerRole();
        static std::string authorization2();
        static std::string verifyLocalPlayer2();
        static std::string savePlayerRole2();

        static std::string executeLoginSDKCallback(const char *result);
        static std::string getLginSDKResult();
        static std::string loginSDKResult;
        static std::string checkPayEnv();
        static std::string executePaySDKCallback(const char *result);
        static std::string queryConsumProdts();
        static std::string buy();
        static std::string queryNoConsumableGoods();
        static std::string purchaseOfNonConsumableGoods();
        static std::string disposableGoodsOrderQuery();
        static std::string nonConsumableProductsRecords();
        static std::string consumableProductsRecords();
        static std::string noConsumGoodsService();
        static std::string queryAutorenewGoods();
        static std::string purchaseOfAutoRenewGoods();
        static std::string autoRenewProductsRecords();
        static std::string autoRenewGoodsService();
        static std::string getPaySDKResult();

        static std::string paySDKResult;
        static MsgCallback msgCb;
    protected:
        static SDKApplication *sm_pSharedApplication;
    };
}
#endif  
