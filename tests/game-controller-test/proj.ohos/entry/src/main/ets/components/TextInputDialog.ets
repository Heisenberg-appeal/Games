import { TextInputDialogEntity } from '@ohos/libSysCapabilities'
import worker from '@ohos.worker';
import { WorkerManager } from '../workers/WorkerManager';

@CustomDialog
export struct TextInputDialog {
  @State showMessage: TextInputDialogEntity = new TextInputDialogEntity('');
  cocosWorker: worker.ThreadWorker = WorkerManager.getInstance().getWorker();
  controller?: CustomDialogController

  build() {
    Column() {
      Row() {
        TextInput({ text: this.showMessage.message })
          .backgroundColor('#ffffff')
          .layoutWeight(1)
          .caretColor(Color.Transparent)
          .onChange((value) => {
            this.cocosWorker.postMessage({type: "textFieldTTFOnChange", data: value})
          })
        Blank(8).width(16)
        Button('完成').onClick(() => {
          this.controller!.close();
        })
      }.padding({ left: 8, right: 8, top: 8, bottom: 8 })
      .backgroundColor(Color.Gray)
    }
    .width('100%')

    .justifyContent(FlexAlign.End)
  }
}