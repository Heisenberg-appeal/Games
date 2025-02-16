/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
import { util } from '@kit.ArkTS';
import { Logger } from './Logger';

const centerLineRegex: RegExp = new RegExp('-', 'g');
const underLineRegex: RegExp = new RegExp('_', 'g');
const textDecoder = util.TextDecoder.create("utf-8", { ignoreBOM: true });
const base64 = new util.Base64Helper();
const TAG: string = 'TextUtil';

const BASE64_PAD_MOD = 4;
const BASE64_PAD_INVALID = 1;

export class JWTUtil {
  private static base64Decode(input: string): string {
    return textDecoder.decodeWithStream(base64.decodeSync(input));
  }

  private static base64UrlDecode(input: string): string {
    input = input.replace(centerLineRegex, '+').replace(underLineRegex, '/');
    const pad = input.length % BASE64_PAD_MOD;
    if (pad) {
      if (pad === BASE64_PAD_INVALID) {
        throw new Error('InvalidLengthError: Input base64url string is the wrong length to determine padding');
      }
      input += new Array(5 - pad).join('=');
    }
    return this.base64Decode(input);
  }

  public static decodeJwtObj(data: string): string {
    let jwt: string[] = data.split('.');
    let result: string = '';
    if (jwt.length < 3) {
      return result;
    }
    try {
      result = JWTUtil.base64UrlDecode(jwt[1]);
    } catch (err) {
      Logger.error(TAG, `decodeJwtObj parse err: ${JSON.stringify(err)}`);
    }
    return result;
  }
}