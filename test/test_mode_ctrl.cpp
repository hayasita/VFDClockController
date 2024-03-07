/**
 * @file test_mode_ctrl.cpp
 * @author hayasita04@gmail.com
 * @brief CtrlMode testfile
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include "../src/mode_ctrl.h"

#define dispModeVfd_Default VFD_DISP_DEFAULT
#define dispModeOled_Default OLED_DISP_SENSOR_DATA
#define dispModeM5Oled_Default M5OLED_DISP_SENSOR_DATA

/**
 * @brief Construct a new TEST object
 * 初期値確認
 * 
 */
TEST(ModeCtrl, ctrlModechg_ini){
  dispMode modeData;
  modeCtrl mode(true,true);

  // 初期値確認
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);            // 操作モード取得：VFD表示
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeM5Oled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  // キー入力なし
  modeData = mode.modeSet(0,0);                               // キー入力なし
//  EXPECT_EQ(modeData.ctrlModeSelect, 0);                      // 操作モード選択　0:モード切替
  EXPECT_EQ(modeData.adjKeyData, 0);                          // 設定操作用キー情報
  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdDisp);             // 操作モード：VFD表示
  EXPECT_EQ(modeData.dispModeVfdCount, dispModeVfd_Default);  // VFD表示モード：標準表示
  EXPECT_EQ(modeData.dispModeVfdCount, 0);                    // VFD表示モードテーブルカウンタ
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード：初期値
  EXPECT_EQ(modeData.dispModeOLEDCount, 0);                   // OLED表示モードテーブルカウンタ
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード：初期値
  EXPECT_EQ(modeData.dispModeM5OLEDCount, 0);                 // M5OLED表示モードテーブルカウンタ
}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * SW1(SET) Long ON
 * OLED あり
 * M5OLED　あり
 */
TEST(ModeCtrl, ctrlModechg_Full){
  dispMode modeData;

  modeCtrl mode(true,true);
  // 初期値確認
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);            // 操作モード取得：VFD表示
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  // SETKey SW1 Long ON -> VFD設定モード
  modeData = mode.modeSet(kEY_SET_L,0);    // -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);            // 操作モード取得：VFD設定
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CLOCK_ADJ);       // VFD表示モード取得：時計調整
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdCtrl);             // 操作モード取得：VFD設定
  EXPECT_EQ(modeData.dispModeVfd, VFD_DISP_CLOCK_ADJ);        // VFD表示モード取得：時計調整
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード取得：初期値

  // SETKey SW1 Long ON ->OLED設定モード
  modeData = mode.modeSet(kEY_SET_L,0);    // -> OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);            // 操作モード取得：OLED設定
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                // 操作モード取得：OLED設定
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);       // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード取得：初期値

  // SETKey SW1 Long ON ->M5OLED設定モード
  modeData = mode.modeSet(kEY_SET_L,0);    // -> M5OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);             // 操作モード取得：M5OLED設定
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);              // 操作モード取得：M5OLED設定
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);       // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード取得：初期値

  // SETKey SW1 Long ON ->VFD表示
  modeData = mode.modeSet(kEY_SET_L,0);    // -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);            // 操作モード取得：VFD表示
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED あり
 * M5OLED　なし
 */
TEST(ModeCtrl, ctrlModechg_OLED){
  dispMode dispModeData;        // 動作モード
  modeCtrl mode(true,false);    // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_CLOCK_ADJ);  // VFD表示モード：時計調整

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_DEFAULT);    // VFD表示モード：標準表示

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：OLED設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_DEFAULT);    // VFD表示モード：標準表示

//  EXPECT_FALSE(false);
}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED なし
 * M5OLED　あり
 */
TEST(ModeCtrl, ctrlModechg_M5OLED){
  dispMode dispModeData;        // 動作モード
  modeCtrl mode(false,true);    // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_CLOCK_ADJ);  // VFD表示モード：時計調整

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> M5OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_DEFAULT);    // VFD表示モード：標準表示

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：M5OLED設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_DEFAULT);    // VFD表示モード：標準表示

}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED なし
 * M5OLED　無し
 */
TEST(ModeCtrl, ctrlModechg_NoOLED){
  dispMode dispModeData;        // 動作モード
  modeCtrl mode(false,false);   // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_CLOCK_ADJ);  // VFD表示モード：時計調整

  dispModeData = mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);
  EXPECT_EQ(dispModeData.dispModeVfd, VFD_DISP_DEFAULT);    // VFD表示モード：標準表示

}

/**
 * @brief Construct a new TEST object
 * getDispModeVfd()状態遷移
 * VFD表示モードにおける、SW2,SW3 Short ON を使用した表示内容変更の状態遷移確認
 * 
 */
TEST(ModeCtrl_sw2sw3, dispModeSetVFD){
  dispMode modeData;
  modeCtrl mode(true,true);

  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // VFD表示モード取得：初期値

  // UP Key Lotate
  mode.modeSetVFD(KEY_UP_S,0);                                // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIMECLOCK);       // 時刻表示
  mode.modeSetVFD(KEY_UP_S,0);                                // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CALENDAR);        // カレンダー表示
  mode.modeSetVFD(KEY_UP_S,0);                                // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIME_SENSOR3);    // 時刻・気温・湿度・気圧表示
  mode.modeSetVFD(KEY_UP_S,0);                                // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TMP);             // 気温表示
  mode.modeSetVFD(KEY_UP_S,0);                                // 表示モードローテーション確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // 初期値

  // DOWN Key Lotate
  mode.modeSetVFD(KEY_DOWN_S,0);                              // 表示モードローテーション確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TMP);             // 気温表示
  mode.modeSetVFD(KEY_DOWN_S,0);                              // 表示モードマイナス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIME_SENSOR3);    // 時刻・気温・湿度・気圧表示

  // VFD Mode Redet
  mode.modeSetVFD(0,SWKEY_DISP_MODE_VFD_CLR);                 // VFD表示モード初期化
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // 初期値

  // adjKeyData setKey
  modeData = mode.modeSet(KEY_SET_S,0);                       // 設定操作用キー情報設定 
  EXPECT_EQ(modeData.adjKeyData, KEY_SET_S);                  // KEY_SET_S

  // control mode to adj
  modeData = mode.modeSet(kEY_SET_L,0);                       // 操作モード変更：VFD表示 -> VFD設定
  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdCtrl);             // VFD設定に遷移

}

/**
 * @brief Construct a new TEST object
 * getDispModeVfd()状態遷移
 * VFD設定モードにおける、SW2,SW3 Short ON を使用した表示内容変更の状態遷移確認
 */
TEST(ModeCtrl_sw2sw3, dispModeSetVFDCtrl){
  dispMode modeData;
  modeCtrl mode(true,true);

  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // VFD表示モード取得：初期値
  mode.modeSet(kEY_SET_L,0);                                  // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);            // VFD設定
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CLOCK_ADJ);       // 時計調整

  // UP Key Lotate
  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CAL_ADJ);         // カレンダー調整

  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CLOCK_1224SEL);   // 12h24h表示切替

  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_FADETIME_ADJ);    // クロスフェード時間設定

  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
//  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_BRIGHTNESS_ADJ);  // VFD輝度調整
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CLOCK_ADJ);       // 時計調整

}

/**
 * @brief Construct a new TEST object
 * getDispModeOLED()状態遷移
 * OLED設定モードにおける、SW2,SW3 Short ON を使用した表示内容変更の状態遷移確認
 */
TEST(ModeCtrl_sw2sw3, dispModeSetOLED){
  dispMode modeData;
  modeCtrl mode(true,true);

  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // VFD表示モード取得：初期値

  mode.modeSet(kEY_SET_L,0);                                  // 操作モード：VFD表示 -> VFD設定
  mode.modeSet(kEY_SET_L,0);                                  // 操作モード：VFD設定 -> OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);               // OLED設定
  EXPECT_EQ(mode.getDispModeOLED(),OLED_DISP_SENSOR_DATA);    // 環境センサデータ表示

  // UP Key Lotate
  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeOLED(), OLED_DISP_ANALOG_DATA);   // アナログ入力センサデータ表示

  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeOLED(), OLED_DISP_EVENTLOG_CTRL); // EventLog操作情報

  mode.modeSet(KEY_UP_S,0);                                   // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeOLED(), OLED_DISP_SENSOR_DATA);   // EventLog操作情報

  // DOWN Key Lotate
  mode.modeSet(KEY_DOWN_S,0);                                 // 表示モードローテーション確認
  EXPECT_EQ(mode.getDispModeOLED(), OLED_DISP_EVENTLOG_CTRL); // EventLog操作情報

}

/**
 * @brief Construct a new TEST object
 * getDispModeM5OLED()状態遷移
 * M5OLED設定モードにおける、SW2,SW3 Short ON を使用した表示内容変更の状態遷移確認
 */
TEST(ModeCtrl_sw2sw3, dispModeSetM5OLED){
  dispMode modeData;
  modeCtrl mode(true,true);

  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);             // VFD表示モード取得：初期値
  mode.modeSet(kEY_SET_L,0);                                      // 操作モード：VFD表示 -> VFD設定
  mode.modeSet(kEY_SET_L,0);                                      // 操作モード：VFD設定 -> OLED設定
  mode.modeSet(kEY_SET_L,0);                                      // 操作モード：OLED設定 -> M5OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);                 // VFD設定
  EXPECT_EQ(mode.getDispModeM5OLED(),M5OLED_DISP_SENSOR_DATA);    // 環境センサデータ表示

  // UP Key Lotate
  mode.modeSet(KEY_UP_S,0);                                       // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeM5OLED(), M5OLED_DISP_EVENTLOG_CTRL); // EventLog操作情報

  mode.modeSet(KEY_UP_S,0);                                       // 表示モードプラス確認
  EXPECT_EQ(mode.getDispModeM5OLED(), M5OLED_DISP_SENSOR_DATA);   // EventLog操作情報

  // DOWN Key Lotate
  mode.modeSet(KEY_DOWN_S,0);                                     // 表示モードローテーション確認
  EXPECT_EQ(mode.getDispModeM5OLED(), M5OLED_DISP_EVENTLOG_CTRL); // EventLog操作情報

}
