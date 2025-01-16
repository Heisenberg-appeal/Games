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
#include "SDKTest.h"
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

SDKTests::SDKTests() { ADD_TEST_CASE(SDKControlTest); }

SDKControlTest::SDKControlTest() {
  cocos2d::SDKApplication::msgCb = onMsg;
  auto label = Label::createWithTTF("SDK Test", "fonts/arial.ttf", 28);
  addChild(label, 0);
  label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 50);

  auto loginInit = TextButton::create("Login Init", [&](TextButton *button) {
    cocos2d::SDKApplication::loginInit();
  });
  loginInit->setPosition(VisibleRect::center().x, VisibleRect::top().y - 100);
  addChild(loginInit);
    
  auto on = TextButton::create("Login on", [&](TextButton *button) {
    cocos2d::SDKApplication::on();
  });
  on->setPosition(VisibleRect::center().x, VisibleRect::top().y - 150);
  addChild(on);
    
  auto unionLogin = TextButton::create("Login unionLogin", [&](TextButton *button) {
    cocos2d::SDKApplication::unionLogin();
  });
  unionLogin->setPosition(VisibleRect::center().x, VisibleRect::top().y - 200);
  addChild(unionLogin);

  auto label1 = Label::createWithTTF("------Quick login with a Huawei account----------", "fonts/arial.ttf", 20);
  addChild(label1, 0);
  label1->setPosition(VisibleRect::center().x, VisibleRect::top().y - 250);

  auto authorization = TextButton::create("Login authorization", [&](TextButton *button) {
    cocos2d::SDKApplication::authorization();
  });
  authorization->setPosition(VisibleRect::center().x, VisibleRect::top().y - 300);
  addChild(authorization);

  auto playerInfoVerification = TextButton::create("Login playerInfoVerification", [&](TextButton *button) {
    cocos2d::SDKApplication::playerInfoVerification();
  });
  playerInfoVerification->setPosition(VisibleRect::center().x, VisibleRect::top().y - 350);
  addChild(playerInfoVerification);

  auto bindPlayer = TextButton::create("Login bindPlayer", [&](TextButton *button) {
    cocos2d::SDKApplication::bindPlayer();
  });
  bindPlayer->setPosition(VisibleRect::center().x, VisibleRect::top().y - 400);
  addChild(bindPlayer);

  auto verifyLocalPlayer = TextButton::create("Login verifyLocalPlayer", [&](TextButton *button) {
    cocos2d::SDKApplication::verifyLocalPlayer();
  });
  verifyLocalPlayer->setPosition(VisibleRect::center().x, VisibleRect::top().y - 450);
  addChild(verifyLocalPlayer);

  auto savePlayerRole = TextButton::create("Login savePlayerRole", [&](TextButton *button) {
    cocos2d::SDKApplication::savePlayerRole();
  });
  savePlayerRole->setPosition(VisibleRect::center().x, VisibleRect::top().y - 500);
  addChild(savePlayerRole);

  auto label2 = Label::createWithTTF("------Log in to the official game account----------", "fonts/arial.ttf", 20);
  addChild(label2, 0);
  label2->setPosition(VisibleRect::center().x, VisibleRect::top().y - 550);

  auto authorization2 = TextButton::create("Login authorization2", [&](TextButton *button) {
    cocos2d::SDKApplication::authorization2();
  });
  authorization2->setPosition(VisibleRect::center().x, VisibleRect::top().y - 600);
  addChild(authorization2);

  auto verifyLocalPlayer2 = TextButton::create("Login verifyLocalPlayer2", [&](TextButton *button) {
    cocos2d::SDKApplication::verifyLocalPlayer2();
  });
  verifyLocalPlayer2->setPosition(VisibleRect::center().x, VisibleRect::top().y - 650);
  addChild(verifyLocalPlayer2);

  auto savePlayerRole2 = TextButton::create("Login savePlayerRole2", [&](TextButton *button) {
    cocos2d::SDKApplication::savePlayerRole2();
  });
  savePlayerRole2->setPosition(VisibleRect::center().x, VisibleRect::top().y - 700);
  addChild(savePlayerRole2);

  auto getResult = TextButton::create("Get Result", [&](TextButton *button) {
      this->removeChildByTag(0);
      auto result = Label::createWithTTF("", "fonts/arial.ttf", 20);
      result->setPosition(VisibleRect::center().x, VisibleRect::top().y - 800);
      std::string sdkResult = "";
      sdkResult = cocos2d::SDKApplication::getLginSDKResult();
      
      result->setString(sdkResult);
      addChild(result,0, 0);
  });

  getResult->setPosition(VisibleRect::center().x, VisibleRect::top().y - 750);
  addChild(getResult);
}
void SDKControlTest::onMsg(const char *result)
{ 
    // 处理返回结果
	std::string str = std::string(result);
    CCLOG("call back msg : '%s'.", str.c_str());
}
 