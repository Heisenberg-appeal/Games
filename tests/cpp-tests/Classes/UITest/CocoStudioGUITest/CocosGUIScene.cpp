/****************************************************************************
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

#include "CocosGUIScene.h"

#include "UIButtonTest/UIButtonTest.h"
#include "UICheckBoxTest/UICheckBoxTest.h"
#include "UIRadioButtonTest/UIRadioButtonTest.h"
#include "UISliderTest/UISliderTest.h"
#include "UIImageViewTest/UIImageViewTest.h"
#include "UILoadingBarTest/UILoadingBarTest.h"
#include "UITextAtlasTest/UITextAtlasTest.h"
#include "UITextTest/UITextTest.h"
#include "UITextBMFontTest/UITextBMFontTest.h"
#include "UITextFieldTest/UITextFieldTest.h"
#include "UILayoutTest/UILayoutTest.h"
#include "UIScrollViewTest/UIScrollViewTest.h"
#include "UIPageViewTest/UIPageViewTest.h"
#include "UIListViewTest/UIListViewTest.h"
#include "UIWidgetAddNodeTest/UIWidgetAddNodeTest.h"
#include "UIRichTextTest/UIRichTextTest.h"
#include "UIFocusTest/UIFocusTest.h"
#include "UITabControlTest/UITabControlTest.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_OHOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_TARGET_OS_TVOS)
#include "UIVideoPlayerTest/UIVideoPlayerTest.h"
#include "UIWebViewTest/UIWebViewTest.h"
#endif
#include "UIScale9SpriteTest.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_OHOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "UIEditBoxTest.h"
#endif

GUIDynamicCreateTests::GUIDynamicCreateTests()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_OHOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_TARGET_OS_TVOS)
    addTest("VideoPlayer Test", [](){ return new (std::nothrow) VideoPlayerTests; });
    addTest("WebView Test", [](){ return new (std::nothrow) WebViewTests; });
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_OHOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    addTest("EditBox Test", [](){ return new (std::nothrow) UIEditBoxTests; });
#endif
    addTest("Focus Test", [](){ return new (std::nothrow) UIFocusTests; });
    addTest("Scale9Sprite Test", [](){ return new (std::nothrow) UIScale9SpriteTests; });
    addTest("Button Test", [](){ return new (std::nothrow) UIButtonTests; });
    addTest("CheckBox Test", [](){ return new (std::nothrow) UICheckBoxTests; });
    addTest("RadioButton Test", [](){ return new (std::nothrow) UIRadioButtonTests; });
    addTest("Slider Test", [](){ return new (std::nothrow) UISliderTests; });
   
    addTest("ImageView Test", [](){ return new (std::nothrow) UIImageViewTests; });
    addTest("LoadingBar Test", [](){ return new (std::nothrow) UILoadingBarTests; });
    addTest("TextAtlas Test", [](){ return new (std::nothrow) UITextAtlasTests; });
    
    addTest("Text Test", [](){ return new (std::nothrow) UITextTests; });
    addTest("TextBMFont Test", [](){ return new (std::nothrow) UITextBMFontTests; });
    addTest("TextField Test", [](){ return new (std::nothrow) UITextFieldTests; });
    addTest("Layout Test", [](){ return new (std::nothrow) UILayoutTests; });
    
    addTest("ScrollView Test", [](){ return new (std::nothrow) UIScrollViewTests; });
    addTest("PageView Test", [](){ return new (std::nothrow) UIPageViewTests; });
    addTest("ListView Test", [](){ return new (std::nothrow) UIListViewTests; });
    
    addTest("WidgetAddNode Test", [](){ return new (std::nothrow) UIWidgetAddNodeTests; });
    addTest("RichText Test", [](){ return new (std::nothrow) UIRichTextTests; });

    addTest("TabControl Test", [](){return new (std::nothrow) UITabControlTests; });
}
