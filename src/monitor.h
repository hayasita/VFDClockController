/**
 * @file monitor.h
 * @author hayasita04@gmail.com
 * @brief シリアルモニタ処理ヘッダ
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef monitor_h
#define monitor_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include <sstream>

class SerialMonitorIO{
  public:
    virtual ~SerialMonitorIO(void) = 0;         // 純粋仮想デストラクタ
    virtual std::string rsv(void) = 0;          // 純粋仮想関数
    virtual uint8_t send(std::string data) = 0; // 純粋仮想関数
};

class codeTbl{
  public:
    std::string code;                   // コード
    std::function<bool()> execCode;     // コード実行処理
    std::string help;                   // help表示データ
};

class SerialMonitor{
  public:
    /**
     * @brief Construct a new Serial Monitor object
     * 
     * @param pSerialMonitorIo 処理で使用するSerialMonitorIOのポインタを渡す。
     * テストのときはmockを渡す。
     * 実処理の場合は、実処理の派生classのポインタを渡す。
     */
    SerialMonitor(SerialMonitorIO* pSerialMonitorIo);   // コンストラクタ
    bool exec(void);                                    // シリアルモニタ実行
    std::vector<std::string> spritCommand(std::string &commandBuf);  // コマンド分割

  private:
    void init(void);                          // 初期化
    bool dummyExec(std::vector<std::string> command);     // コマンド実行ダミー

    SerialMonitorIO *monitorIo_ = nullptr;    // シリアル入出力処理ポインタ
    std::vector<std::string> command;         // シリアルモニタコマンド
    std::vector<codeTbl> codeArray;           // コードテーブル
    
};

#undef GLOBAL
#endif