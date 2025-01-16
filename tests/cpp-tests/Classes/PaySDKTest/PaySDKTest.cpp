/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "PaySDKTest.h"
#include "CCSDKApplication.h"

USING_NS_CC;

namespace {
class TextButton : public cocos2d::Label {
public:
  static TextButton *
  create(const std::string &text,
         const std::function<void(TextButton *)> &onTriggered) {
    auto ret = new (std::nothrow) TextButton();

    TTFConfig ttfconfig("fonts/arial.ttf", 25);
    if (ret && ret->setTTFConfig(ttfconfig)) {
      ret->setString(text);
      ret->_onTriggered = onTriggered;

      ret->autorelease();

      return ret;
    }

    delete ret;
    return nullptr;
  }

  void setEnabled(bool enabled) {
    _enabled = enabled;
    if (_enabled) {
      this->setColor(Color3B::WHITE);
    } else {
      this->setColor(Color3B::GRAY);
    }
  }

private:
  TextButton() : _enabled(true), _onTriggered(nullptr) {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(TextButton::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(TextButton::onTouchEnded, this);
    listener->onTouchCancelled =
        CC_CALLBACK_2(TextButton::onTouchCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
  }

  bool touchHits(Touch *touch) {
    auto hitPos = this->convertToNodeSpace(touch->getLocation());
    if (hitPos.x >= 0 && hitPos.y >= 0 && hitPos.x <= _contentSize.width &&
        hitPos.y <= _contentSize.height) {
      return true;
    }
    return false;
  }

  bool onTouchBegan(Touch *touch, Event *event) {
    auto hits = touchHits(touch);
    if (hits) {
      scaleButtonTo(0.95f);
    }
    return hits;
  }

  void onTouchEnded(Touch *touch, Event *event) {
    if (_enabled) {
      auto hits = touchHits(touch);
      if (hits && _onTriggered) {
        _onTriggered(this);
      }
    }

    scaleButtonTo(1);
  }

  void onTouchCancelled(Touch *touch, Event *event) { scaleButtonTo(1); }

  void scaleButtonTo(float scale) {
    auto action = ScaleTo::create(0.05f, scale);
    action->setTag(10000);
    stopActionByTag(10000);
    runAction(action);
  }

  std::function<void(TextButton *)> _onTriggered;

  bool _enabled;
};
} // namespace

PaySDKTests::PaySDKTests() { ADD_TEST_CASE(PaySDKControlTest); }

PaySDKControlTest::PaySDKControlTest() {
  auto label = Label::createWithTTF("Pay SDK Test", "fonts/arial.ttf", 28);
  addChild(label, 0);
  label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 50);

    //检查支付环境
  auto sdkItem = TextButton::create("Checking the Payment Environment", [&](TextButton *button) {
    cocos2d::SDKApplication::checkPayEnv();
  });
  sdkItem->setPosition(VisibleRect::center().x, VisibleRect::top().y - 150);
  addChild(sdkItem);
    
    //查询消耗商品
  auto sdkItem1 = TextButton::create("Querying Consumable Products", [&](TextButton *button) {
    cocos2d::SDKApplication::queryConsumProdts();
  });
  sdkItem1->setPosition(VisibleRect::center().x, VisibleRect::top().y - 200);
  addChild(sdkItem1);

  // 查询非消耗型商品
  auto sdkItem2 = TextButton::create("Query non-consumable goods",
                                     [&](TextButton *button) { cocos2d::SDKApplication::queryNoConsumableGoods(); });
  sdkItem2->setPosition(VisibleRect::center().x, VisibleRect::top().y - 250);
  addChild(sdkItem2);

  // 查询订阅型商品信息
  auto sdkItem3 = TextButton::create("Querying Subscription Offering Information",
                                      [&](TextButton *button) { cocos2d::SDKApplication::queryAutorenewGoods(); });
  sdkItem3->setPosition(VisibleRect::center().x, VisibleRect::top().y - 300);
  addChild(sdkItem3);

  // 消耗型商品购买
  auto sdkItem4 = TextButton::create("Purchase of consumable goods", [&](TextButton *button) {
    cocos2d::SDKApplication::buy();
  });
  sdkItem4->setPosition(VisibleRect::center().x, VisibleRect::top().y - 350);
  addChild(sdkItem4);
    
    //非消耗型商品购买
  auto sdkItem5 = TextButton::create("Purchase of non-consumable goods", [&](TextButton *button) {
    cocos2d::SDKApplication::purchaseOfNonConsumableGoods();
  });
  sdkItem5->setPosition(VisibleRect::center().x, VisibleRect::top().y - 400);
  addChild(sdkItem5);

  // 订阅型商品购买
  auto sdkItem6 = TextButton::create("Subscription-based purchase",
                                      [&](TextButton *button) { cocos2d::SDKApplication::purchaseOfAutoRenewGoods(); });
  sdkItem6->setPosition(VisibleRect::center().x, VisibleRect::top().y - 450);
  addChild(sdkItem6);

  // 查询消耗型商品的购买记录
  auto sdkItem7 = TextButton::create("Querying Purchase Records of Consumable Products",
                                     [&](TextButton *button) { cocos2d::SDKApplication::consumableProductsRecords(); });
  sdkItem7->setPosition(VisibleRect::center().x, VisibleRect::top().y - 500);
  addChild(sdkItem7);

  // 查询非消耗型商品的购买记录
  auto sdkItem8 = TextButton::create("Querying Purchase Records of Non-consumable Products", [&](TextButton *button) {
      cocos2d::SDKApplication::nonConsumableProductsRecords();
  });
  sdkItem8->setPosition(VisibleRect::center().x, VisibleRect::top().y - 550);
  addChild(sdkItem8);

  // 查询订阅型商品的购买记录
  auto sdkItem9 = TextButton::create("Querying Subscription Records",
                                      [&](TextButton *button) { cocos2d::SDKApplication::autoRenewProductsRecords(); });
  sdkItem9->setPosition(VisibleRect::center().x, VisibleRect::top().y - 600);
  addChild(sdkItem9);

  // 消耗型商品掉单查询
  auto sdkItem10 = TextButton::create("Disposable goods order query", [&](TextButton *button) {
    cocos2d::SDKApplication::disposableGoodsOrderQuery();
  });
  sdkItem10->setPosition(VisibleRect::center().x, VisibleRect::top().y - 650);
  addChild(sdkItem10);

  // 非消耗型商品提供服务
  auto sdkItem11 = TextButton::create("Non-expendable goods provision service",
                                     [&](TextButton *button) { cocos2d::SDKApplication::noConsumGoodsService(); });
  sdkItem11 ->setPosition(VisibleRect::center().x, VisibleRect::top().y - 700);
  addChild(sdkItem11);
    
  // 订阅型商品提供服务
  auto sdkItem12 = TextButton::create("Subscription-based offering",
                                      [&](TextButton *button) { cocos2d::SDKApplication::autoRenewGoodsService(); });
  sdkItem12->setPosition(VisibleRect::center().x, VisibleRect::top().y - 750);
  addChild(sdkItem12);

  auto getResult = TextButton::create("Get Result", [&](TextButton *button) {
    this->removeChildByTag(0);
    auto result = Label::createWithTTF("", "fonts/arial.ttf", 20);
    result->setPosition(VisibleRect::center().x, VisibleRect::top().y - 850);
    std::string sdkResult = "";
    sdkResult = cocos2d::SDKApplication::getPaySDKResult();
    result->setString(sdkResult);
    addChild(result,0, 0);
  });
  getResult->setPosition(VisibleRect::center().x, VisibleRect::top().y - 800);
  addChild(getResult);
}