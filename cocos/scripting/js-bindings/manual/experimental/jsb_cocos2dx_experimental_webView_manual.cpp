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

#include "scripting/js-bindings/manual/experimental/jsb_cocos2dx_experimental_webView_manual.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_OHOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "ui/UIWebView.h"
#include "scripting/js-bindings/manual/ScriptingCore.h"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"

using namespace cocos2d;


static bool jsb_cocos2dx_experimental_webView_setOnShouldStartLoading(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    experimental::ui::WebView* cobj = (experimental::ui::WebView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");

    if(argc == 1){
        std::shared_ptr<JSFunctionWrapper> func(new JSFunctionWrapper(cx, obj, args.get(0), args.thisv()));
        cobj->setOnShouldStartLoading([=](experimental::ui::WebView *sender, const std::string &url)->bool{
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            jsval arg[2];
            JS::RootedObject jsobj(cx, js_get_or_create_jsobject<experimental::ui::WebView>(cx, sender));
            arg[0] = OBJECT_TO_JSVAL(jsobj);
            arg[1] = std_string_to_jsval(cx, url);
            JS::RootedValue rval(cx);

            bool ok = func->invoke(2, arg, &rval);
            if (!ok && JS_IsExceptionPending(cx)) {
                JS_ReportPendingException(cx);
            }
            return rval.toBoolean();
        });
        return true;
    }

    JS_ReportError(cx, "jsb_cocos2dx_experimental_webView_setOnShouldStartLoading : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

static bool jsb_cocos2dx_experimental_webView_setOnDidFinishLoading(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    experimental::ui::WebView* cobj = (experimental::ui::WebView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");

    if(argc == 1){
        std::shared_ptr<JSFunctionWrapper> func(new JSFunctionWrapper(cx, obj, args.get(0), args.thisv()));
        cobj->setOnDidFinishLoading([=](experimental::ui::WebView *sender, const std::string &url)->void{
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            jsval arg[2];
            JS::RootedObject jsobj(cx, js_get_or_create_jsobject<experimental::ui::WebView>(cx, sender));
            arg[0] = OBJECT_TO_JSVAL(jsobj);
            arg[1] = std_string_to_jsval(cx, url);
            JS::RootedValue rval(cx);

            bool ok = func->invoke(2, arg, &rval);
            if (!ok && JS_IsExceptionPending(cx)) {
                JS_ReportPendingException(cx);
            }
        });
        return true;
    }

    JS_ReportError(cx, "jsb_cocos2dx_experimental_webView_setOnDidFinishLoading : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

static bool jsb_cocos2dx_experimental_webView_setOnDidFailLoading(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    experimental::ui::WebView* cobj = (experimental::ui::WebView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");

    if(argc == 1){
        std::shared_ptr<JSFunctionWrapper> func(new JSFunctionWrapper(cx, obj, args.get(0), args.thisv()));
        cobj->setOnDidFailLoading([=](experimental::ui::WebView *sender, const std::string &url)->void{
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            jsval arg[2];
            JS::RootedObject jsobj(cx, js_get_or_create_jsobject<experimental::ui::WebView>(cx, sender));
            arg[0] = OBJECT_TO_JSVAL(jsobj);
            arg[1] = std_string_to_jsval(cx, url);
            JS::RootedValue rval(cx);

            bool ok = func->invoke(2, arg, &rval);
            if (!ok && JS_IsExceptionPending(cx)) {
                JS_ReportPendingException(cx);
            }
        });
        return true;
    }

    JS_ReportError(cx, "jsb_cocos2dx_experimental_webView_setOnDidFailLoading : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

static bool jsb_cocos2dx_experimental_webView_setOnJSCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    experimental::ui::WebView* cobj = (experimental::ui::WebView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");

    if(argc == 1){
        std::shared_ptr<JSFunctionWrapper> func(new JSFunctionWrapper(cx, obj, args.get(0), args.thisv()));
        cobj->setOnJSCallback([=](experimental::ui::WebView *sender, const std::string &url)->void{
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            jsval arg[2];
            JS::RootedObject jsobj(cx, js_get_or_create_jsobject<experimental::ui::WebView>(cx, sender));
            arg[0] = OBJECT_TO_JSVAL(jsobj);
            arg[1] = std_string_to_jsval(cx, url);
            JS::RootedValue rval(cx);

            bool ok = func->invoke(2, arg, &rval);
            if (!ok && JS_IsExceptionPending(cx)) {
                JS_ReportPendingException(cx);
            }
        });
        return true;
    }

    JS_ReportError(cx, "jsb_cocos2dx_experimental_webView_setOnJSCallback : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
extern JSObject* jsb_cocos2d_experimental_ui_WebView_prototype;

void register_all_cocos2dx_experimental_webView_manual(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject proto(cx, jsb_cocos2d_experimental_ui_WebView_prototype);
    JS_DefineFunction(cx, proto, "setOnShouldStartLoading", jsb_cocos2dx_experimental_webView_setOnShouldStartLoading, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, proto, "setOnDidFinishLoading", jsb_cocos2dx_experimental_webView_setOnDidFinishLoading, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, proto, "setOnDidFailLoading", jsb_cocos2dx_experimental_webView_setOnDidFailLoading, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, proto, "setOnJSCallback", jsb_cocos2dx_experimental_webView_setOnJSCallback, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
}

#endif
