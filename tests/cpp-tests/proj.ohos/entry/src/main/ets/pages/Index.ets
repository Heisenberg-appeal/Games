import nativeRender from 'libnativerender.so'
import { ContextType } from '@ohos/libSysCapabilities'
import { TextInputInfo } from '@ohos/libSysCapabilities/src/main/ets/components/editbox/EditBoxMsg'
import { TextInputDialogEntity } from '@ohos/libSysCapabilities'
import { WebViewInfo } from '@ohos/libSysCapabilities/src/main/ets/components/webview/WebViewMsg'
import { VideoPlayerInfo } from '@ohos/libSysCapabilities/src/main/ets/components/videoplayer/VideoPlayerMsg'
import { WorkerMsgUtils } from '@ohos/libSysCapabilities/src/main/ets/utils/WorkerMsgUtils'
import { WorkerManager } from '../workers/WorkerManager'
import { CocosEditBox } from '../components/CocosEditBox'
import { CocosWebView } from '../components/CocosWebview'
import { CocosVideoPlayer } from '../components/CocosVideoPlayer'
import { TextInputDialog } from '../components/TextInputDialog'
import { EventData, sdkManagerMsg } from "@ohos/libSysSDKCapabilities"
import { GlobalContext, GlobalContextConstants } from "@ohos/libSysCapabilities"
import { MessageEvents } from '@ohos.worker'
import common from '@ohos.app.ability.common'
import deviceInfo from '@ohos.deviceInfo';
import promptAction from '@ohos.promptAction';
import process from '@ohos.process';

const nativePageLifecycle: nativeRender.CPPFunctions = nativeRender.getContext(ContextType.JSPAGE_LIFECYCLE);

let cocosWorker = WorkerManager.getInstance().getWorker();

@Entry
@Component
struct Index {
  xcomponentController: XComponentController = new XComponentController();

  processMgr = new process.ProcessManager();


  // EditBox
  @State editBoxArray: TextInputInfo[] = [];
  private editBoxIndexMap: Map<number, TextInputInfo> = new Map;
  // WebView
  @State webViewArray: WebViewInfo[] = [];
  private webViewIndexMap: Map<number, number> = new Map;
  // videoPlayer
  @State videoPlayerInfoArray: VideoPlayerInfo[] = [];
  private videoPlayerIndexMap: Map<number, VideoPlayerInfo> = new Map;
  // textInputDialog
  showMessage: TextInputDialogEntity = new TextInputDialogEntity('');
  dialogController: CustomDialogController = new CustomDialogController({
    builder: TextInputDialog({
      showMessage: this.showMessage
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    customStyle: true,
  })
  // PanGesture
  private panOption: PanGestureOptions = new PanGestureOptions({ direction: PanDirection.Up | PanDirection.Down });

  onPageShow() {
    console.log('[LIFECYCLE-Page] onPageShow');
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_EDIT_BOX_ARRAY, this.editBoxArray);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_EDIT_BOX_INDEX_MAP, this.editBoxIndexMap);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_COCOS_WORKER, cocosWorker);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_WEB_VIEW_ARRAY, this.webViewArray);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_WEB_VIEW_INDEX_MAP, this.webViewIndexMap);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_VIDEO_PLAYER_ARRAY, this.videoPlayerInfoArray);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_VIDEO_PLAYER_INDEX_MAP, this.videoPlayerIndexMap);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_DIALOG_CONTROLLER, this.dialogController);
    GlobalContext.storeGlobalThis(GlobalContextConstants.COCOS2DX_SHOW_MESSAGE, this.showMessage);
    nativePageLifecycle.onPageShow();
  }

  onPageHide() {
    console.log('[LIFECYCLE-Page] onPageHide');
    nativePageLifecycle.onPageHide();
  }

  onBackPress() {
    console.log('[LIFECYCLE-Page] onBackPress');
    try {
      promptAction.showDialog({
        title: "提示",
        message: "确认退出游戏吗",
        buttons: [
          {
            text: '取消',
            color: '#000000'
          },
          {
            text: '确认',
            color: '#000000'
          }
        ],
      }).then(data => {
        console.info('showDialog success, click button: ' + data.index);
        if(data.index == 0) {
          console.info('showDialog click button cancel');
          return;
        } else {
          console.info('showDialog click button ok');
          this.processMgr.exit(0);
        }
      })
    } catch (error) {
      console.error(`showDialog args error code is ${error.code}, message is ${error.message}`);
    };
    // If disable system exit needed, remove comment "return true"
    return true;
  }

  onMouseWheel(eventType: string, scrollY: number) {
    cocosWorker.postMessage({ type: "onMouseWheel", eventType: eventType, scrollY: scrollY });
  }

  build() {
    Stack() {
      XComponent({
        id: 'xcomponentId',
        type: 'surface',
        libraryname: 'nativerender',
        controller: this.xcomponentController
      })
        .focusable(true)
        .focusOnTouch(true)
        .gesture(
          PanGesture(this.panOption)
            .onActionStart(() => {
              this.onMouseWheel("actionStart", 0);
            })
            .onActionUpdate((event: GestureEvent) => {
              if (deviceInfo.deviceType === '2in1') {
                this.onMouseWheel("actionUpdate", event.offsetY);
              }
            })
            .onActionEnd(() => {
              this.onMouseWheel("actionEnd", 0);
            })
        )
        .onLoad((context) => {
          console.log('[cocos] XComponent.onLoad Callback');
          cocosWorker.postMessage({ type: "abilityContextInit", data: GlobalContext.loadGlobalThis(GlobalContextConstants.COCOS2DX_ABILITY_CONTEXT)});
          cocosWorker.postMessage({ type: "onXCLoad", data: "XComponent" });
          cocosWorker.onmessage = async (event: MessageEvents) => {
            let eventData: EventData = event.data;
            switch (eventData.module) {
              case 'LoginSDK':
                let result: string;
                try {
                  result = await sdkManagerMsg.handlePullSDK(eventData as EventData);
                } catch (e) {
                  result =JSON.stringify(e);
                }
                console.log('！！！！！！！！！！！！！login success: %{public}s', JSON.stringify(result));
                if (result != null || result != undefined) {
                  console.log('！！！！！！！！！！！！！login cocosWorker postMessage start ' );
                  try{
                    cocosWorker.postMessage({type:"syncLoginSDKResult", data: result});
                  }catch (e){
                    console.log('！！！！！！！！！！！！！login  cocosWorker postMessage error %{public}s',e );
                  }
                  console.log('！！！！！！！！！！！！！login  cocosWorker postMessage end ' );
                }
                break;
              case 'PaySDK':
                let paySDKResult: string;
                try {
                  paySDKResult = await sdkManagerMsg.handlePullSDK(eventData as EventData);
                } catch (e) {
                  paySDKResult =JSON.stringify(e);
                }
                console.log('！！！！！！！！！！！！！check pay env success: %{public}s', JSON.stringify(paySDKResult));
                if (paySDKResult != null || paySDKResult != undefined) {
                  console.log('！！！！！！！！！！！！！  cocosWorker postMessage start ' );
                  try{
                    cocosWorker.postMessage({type:"syncPaySDKResult", data: paySDKResult});
                  }catch (e){
                    console.log('！！！！！！！！！！！！！ cocosWorker postMessage error %{public}s',e );
                  }
                  console.log('！！！！！！！！！！！！！ cocosWorker postMessage end ' );
                }
                break;
              default:
                 WorkerMsgUtils.recvWorkerThreadMessage(event);
            }
          };
        })
        .onDestroy(() => {
        })
      ForEach(this.editBoxArray, (item:TextInputInfo) => {
        CocosEditBox({ textInputInfo: item});
      }, (item:TextInputInfo) => item.viewTag.toString())

      ForEach(this.webViewArray, (item:WebViewInfo) => {
        CocosWebView({ viewInfo: item })
      }, (item:WebViewInfo) => item.uniqueId.toString())

      ForEach(this.videoPlayerInfoArray, (item:VideoPlayerInfo) => {
        CocosVideoPlayer({ videoPlayerInfo: item })
      }, (item:VideoPlayerInfo) => item.viewTag.toString())
    }
    .width('100%')
    .height('90%')
  }
}
