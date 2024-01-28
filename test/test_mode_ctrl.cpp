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
  EXPECT_EQ(mode.getDispModeVfdCtrl(), 0);                    // VFD設定表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  modeData = mode.modeSet(0,0);
  EXPECT_EQ(modeData.ctrlModeSelect, 0);                      // 操作モード選択　0:モード切替
  EXPECT_EQ(modeData.adjKeyData, 0);                          // 設定操作用キー情報
  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdDisp);             // 操作モード

  EXPECT_EQ(modeData.dispModeVfdCount, VFD_DISP_DEFAULT);     // VFD表示モード初期化
  EXPECT_EQ(modeData.dispModeVfdCount, 0);                    // VFD表示モードテーブルカウンタ

  EXPECT_EQ(modeData.dispModeVfdCtrl, 0);                     //VFD設定表示モード
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード初期化
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード初期化
}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
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
/*
  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdDisp);             // 操作モード取得：VFD表示
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);       // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);   // M5OLED表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);     // OLED表示モード取得：初期値
*/
  // SETKey SW1 Long ON
  modeData = mode.modeSet(kEY_SET_L,0);    // -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);            // 操作モード取得：VFD設定
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);    // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdCtrl);            // 操作モード取得：VFD設定
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);      // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_Default);  // M5OLED表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);    // OLED表示モード取得：初期値

  // SETKey SW1 Long ON
  modeData = mode.modeSet(kEY_SET_L,0);    // -> M5OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);               // 操作モード取得：M5OLED設定
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);        // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData); // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);      // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);                // 操作モード取得：M5OLED設定
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);         // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);  // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);       // OLED表示モード取得：初期値

    // ▲Key SW2 Short ON
    modeData = mode.modeSet(KEY_UP_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);                 // 操作モード取得：M5OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_EventLogCtrl); // M5OLED表示モード取得：EventLog操作情報
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);        // OLED表示モード取得：初期値

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);                  // 操作モード取得：M5OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_EventLogCtrl);  // M5OLED表示モード取得：EventLog操作情報
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);         // OLED表示モード取得：初期値

    // ▲Key SW2 Short ON
    modeData = mode.modeSet(KEY_UP_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);                 // 操作モード取得：M5OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);        // OLED表示モード取得：初期値

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);                  // 操作モード取得：M5OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);         // OLED表示モード取得：初期値

    // ▼Key SW3 Short ON
    modeData = mode.modeSet(KEY_DOWN_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);                 // 操作モード取得：M5OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_EventLogCtrl); // M5OLED表示モード取得：EventLog操作情報
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);        // OLED表示モード取得：初期値

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);                  // 操作モード取得：M5OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_EventLogCtrl);  // M5OLED表示モード取得：EventLog操作情報
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);         // OLED表示モード取得：初期値

    // ▼Key SW3 Short ON
    modeData = mode.modeSet(KEY_DOWN_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);                 // 操作モード取得：M5OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);        // OLED表示モード取得：初期値

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_M5oled);                  // 操作モード取得：M5OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);         // OLED表示モード取得：初期値

  // SETKey SW1 Long ON
  modeData = mode.modeSet(kEY_SET_L,0);    // 操作モード：-> OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);                 // 操作モード取得：OLED設定
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);        // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData); // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);      // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                  // 操作モード取得：OLED設定
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);         // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);  // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);       // OLED表示モード取得：初期値

    // ▲Key SW2 Short ON
    modeData = mode.modeSet(KEY_UP_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);                   // 操作モード取得：OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_EventLogCtrl);   // OLED表示モード取得：EventLog操作情報

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                    // 操作モード取得：OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_EventLogCtrl);    // OLED表示モード取得：EventLog操作情報

    // ▲Key SW2 Short ON
    modeData = mode.modeSet(KEY_UP_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);                   // 操作モード取得：OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_SensorData);

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                    // 操作モード取得：OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_SensorData);

    // ▼Key SW3 Short ON
    modeData = mode.modeSet(KEY_DOWN_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);                   // 操作モード取得：OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_EventLogCtrl);   // OLED表示モード取得：EventLog操作情報

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                    // 操作モード取得：OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_EventLogCtrl);    // OLED表示モード取得：EventLog操作情報

    // ▼Key SW3 Short ON
    modeData = mode.modeSet(KEY_DOWN_S,0);
    EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);                   // 操作モード取得：OLED設定
    EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);          // VFD表示モード取得：初期値
    EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData);   // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_SensorData);

    EXPECT_EQ(modeData.ctrlMode, ctrlMode_Oled);                    // 操作モード取得：OLED設定
    EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);           // VFD表示モード取得：初期値
    EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);    // M5OLED表示モード取得：環境センサデータ表示
    EXPECT_EQ(modeData.dispModeOLED, dispModeOled_SensorData);

  // SETKey SW1 Long ON
  modeData = mode.modeSet(kEY_SET_L,0);    // 操作モード：-> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);              // 操作モード取得：VFD表示
  EXPECT_EQ(mode.getDispModeVfd(), dispModeVfd_Default);        // VFD表示モード取得：初期値
  EXPECT_EQ(mode.getDispModeM5OLED(), dispModeOled_SensorData); // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(mode.getDispModeOLED(), dispModeOled_Default);      // OLED表示モード取得：初期値

  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdDisp);               // 操作モード取得：VFD表示
  EXPECT_EQ(modeData.dispModeVfd, dispModeVfd_Default);         // VFD表示モード取得：初期値
  EXPECT_EQ(modeData.dispModeM5OLED, dispModeOled_SensorData);  // M5OLED表示モード取得：環境センサデータ表示
  EXPECT_EQ(modeData.dispModeOLED, dispModeOled_Default);       // OLED表示モード取得：初期値

}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED あり
 * M5OLED　なし
 */
TEST(ModeCtrl, ctrlModechg_OLED){
  modeCtrl mode(true,false);    // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_Oled);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：OLED設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

//  EXPECT_FALSE(false);
}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED なし
 * M5OLED　あり
 */
TEST(ModeCtrl, ctrlModechg_M5OLED){
  modeCtrl mode(false,true);    // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> M5OLED設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_M5oled);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：M5OLED設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

}

/**
 * @brief Construct a new TEST object
 * CtrlMode状態遷移確認
 * OLED なし
 * M5OLED　無し
 */
TEST(ModeCtrl, ctrlModechg_Mini){
  modeCtrl mode(false,false);   // 操作モード：VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD表示 -> VFD設定
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdCtrl);

  mode.modeSet(kEY_SET_L,0);      // 操作モード：VFD設定 -> VFD表示
  EXPECT_EQ(mode.getCtrlMode(), ctrlMode_VfdDisp);

}

/**
 * @brief Construct a new TEST object
 * getDispModeVfd()状態遷移
 * 
 */
TEST(ModeCtrl, modeSetVFD){
  dispMode modeData;
  modeCtrl mode(true,true);

  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // VFD表示モード取得：初期値

  // UP Key Lotate
  mode.modeSetVFD(KEY_UP_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIMECLOCK);       // 表示モードプラス確認
  mode.modeSetVFD(KEY_UP_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_CALENDAR);        // 表示モードプラス確認
  mode.modeSetVFD(KEY_UP_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIME_SENSOR3);    // 表示モードプラス確認
  mode.modeSetVFD(KEY_UP_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TMP);             // 表示モードプラス確認
  mode.modeSetVFD(KEY_UP_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // 表示モードローテーション確認

  // DOWN Key Lotate
  mode.modeSetVFD(KEY_DOWN_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TMP);             //  表示モードローテーション確認
  mode.modeSetVFD(KEY_DOWN_S,0);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_TIME_SENSOR3);    //  表示モードマイナス確認

  // VFD Mode Redet
  mode.modeSetVFD(0,SWKEY_DISP_MODE_VFD_CLR);
  EXPECT_EQ(mode.getDispModeVfd(), VFD_DISP_DEFAULT);         // VFD表示モード初期化

  // adjKeyData setKey
  modeData = mode.modeSet(KEY_SET_S,0);
  EXPECT_EQ(modeData.adjKeyData, KEY_SET_S);                  // 設定操作用キー情報設定 

  // control mode to adj
  modeData = mode.modeSet(kEY_SET_L,0);
  EXPECT_EQ(modeData.ctrlMode, ctrlMode_VfdCtrl);             // 操作モード：VFD表示 -> VFD設定 

}

