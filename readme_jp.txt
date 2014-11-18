----------------------------------------------------------------------
1. はじめに。
　volca sample SDK を使って、volca sampleにサンプルデータを転送する機能を
　自作のアプリケーションに組み入れることが出来ます。

　volca sampleは、最大100個のサンプル・サウンドをリアルタイムに
　エディットしながらシーケンスを組んでいくことで、強力なライブ・パフォーマンスが
　できるサンプル・シーケンサーです。
　製品についての情報は、コルグ・ホームページ(http://www.korg.com/)を
　参照してください。

　syrostreamとは？
　　volca sampleに転送することができる音声データ(いわゆるピーガー音)を
　　こう呼びます。

　syrodataとは？
　　volca sampleが受信でき、syrostreamに変換する前のデータをこう呼びます。
　　サンプルデータ、パターンデータ、全サンプルデータ、があります。


----------------------------------------------------------------------
2. フォルダ構成

　volca sample SDKには、以下のフォルダがあります。
　　syro    : syro SDK本体で、syrostreamを生成するためのファイルがあります。
　　example : syro SDKを使って、syrostreamをWAV形式で保存するサンプルです。
　　project : syro, exampleフォルダの内容をビルドするためのプロジェクトです。
              visual studio 2010用のプロジェクトファイル、及びgcc,clang用(*1)のMakefileが含まれます。
　　pattern : パターンデータの構造の定義、及び、パターンデータを初期化する関数があります。
　　alldata : 全サンプルを一括で変更する時に送信するファイルがあります。
　　　　　　　工場出荷状態のサンプルセット及び全消去用のデータがあります。

    *1 : clangの時はmake CC=clang とします。

----------------------------------------------------------------------
3. SDK を使って SyroStreamを作成または再生するには

　volca sample SDKを使って、syrodataをsyrostreamに変換することができます。


3.1 使用するソースファイル。

　syro フォルダにある
　　korg_syro_volcasample.c
　　korg_syro_func.c
　　korg_syro_comp.c
　の３つのファイルが必要です。

　syroの関数を呼び出したいソースファイルには、
  #include "korg_syro_volcasample.h"
　を追加してください。


3.2 呼び出しの手順(概要)

　変換は以下の手順で行います。

　a.syrodata(変換したいデータ)を準備します。
　b.変換開始の関数を呼びます。
　c.syrostreamのサンプルデータを1フレーム分づつ取得します。
　d.変換終了の関数を呼びます。

　次項以下で、それぞれの詳細を説明します。
　呼び出しの手順については、 korg_syro_volcasample_example.c も参考にしてください。


3.3 呼び出しの手順について(a)
　a.syrodata(変換したいデータ)を準備します。

　　変換可能なデータは、サンプル及びパターンです。
　　また、サンプルの消去を指定することもできます。この場合はsyrodataは不要です。
　　一度に複数のデータを送るためのsyrostreamを生成することも可能です。

　　データを準備したら、SyroData構造体に必要な情報をセットします。
    　SyroDataType DataType;
　　　　syrodataの種別を指定します。
　　    DataType_Sample_Compress 　　: 単一サンプルデータの変換
　	DataType_Sample_Erase    　　: 単一サンプルデータの消去
　　　　DataType_Sample_AllCompress　: 全サンプルデータの変換
　　　　DataType_Pattern         　　: パターンデータの変換
　　　　が指定可能です。
　　　　単一サンプルデータを変換する場合、サンプル形式は「16ビット」「モノラル」限定となります。
　　　　24ビット等のサンプルを変換したい場合は、16ビットに変換してからsyro を呼び出してください。
　　　　全サンプルデータの変換の場合は、volca sample SDKのalldataにある、拡張子が.alldataファイルのみ指定可能です。

　　　uint8_t *pData;
	syrodataのポインタを指定します。
　　　　サンプルを消去する場合は参照されません、

　　　uint32_t Number;
　　　　サンプル、または、パターンの番号を指定します。
　　　　サンプルの場合は 0~99を指定可能です。
　　　　パターンの場合は 0~9を指定可能です。
　　　　volca sample上のパターン番号から1引いた値を指定してください。

　　　uint32_t Size;
　　　　syrodataのバイトサイズを指定します。
　　　　サンプルデータの場合も含め、全てのデータでバイトサイズになります。
　　　　パターンデータの場合は、0xA40固定値を設定してください。
　　　　(volcasample_pattern.hをincludeすれば、sizeof(VolcaSample_Pattern_Data)が同値になります)
　　　　サンプルを消去する場合は参照されません。

　　　uint32_t Quality;
　　　　サンプルの有効ビット数を8~16で指定します。
　　　　サンプルデータを転送する場合、ビット数を落とすことで転送時間を短くすることが出来ます。
　　　　ここで指定する数値に関わらず、準備するサンプルデータは16ビットである必要があります。
　　　　また、volca sample上でも16ビットで保存するので、保存するサンプル時間には影響はありません。
　　　　ビット数を落とすメリットは、転送時間を短くすることのみです。
　　　　サンプルを消去する場合、及び、パターンを変換する場合は参照されません。

　　　Endian SampleEndian;
　　　　準備したサンプルデータのエンディアンを指定します。
　　　　LittleEndianまたはBigEndianを指定します。
　　　　単一サンプルの変換時のみ参照されます。

　　複数のデータを送りたい場合は、SyroData構造体を配列にして、それぞれに
　　上記の内容をセットしてください。
　　同時に変換できるデータ数は110個です。


3.3 呼び出しの手順について(b)
　b.変換開始の関数を呼びます。

　　必要な情報をセットしたら、開始関数を呼びます。

　　SyroStatus SyroVolcaSample_Start(
　　　　SyroHandle *pHandle, 
　　　　SyroData *pData, 
　　　　int NumOfData,
	uint32_t Flags, 
　　　　uint32_t *pNumOfSyroFrame
　　);

　　引数の内容は、以下のようになります。
　　　　SyroHandle *pHandle　　　　[out] 以降の変換を行う際に指定するハンドルを取得するポインタを指定します。
　　　　SyroData *pData　　　　　　[in]  3.2で準備したSyroData構造体(またはその配列)へのポインタを指定します。
　　　　int NumOfData　　　　　　　[in]  送りたいデータの数(=準備したsyrodataの数)を指定します。
	uint32_t Flags 　　　　　　[in]  変換する時のフラグを指定します。現在は使われないので０を入れておいてください。
　　　　uint32_t *pNumOfSyroFrame　[out] 変換後のSyroDataのサイズを取得するポインタを指定します。
　　　　　　　　　　　　　　　　　 　　　単位は、Frame数(LR１組で1)です。

　　成功した場合は、戻り値にStatus_Successが返ります。
　　以降、変換を終了/中断する場合は、必ずSyroVolcaSample_End関数を呼んでください。

　　失敗した場合のエラー内容については、 3.A を参照してください。


3.3 呼び出しの手順について(c)
　c.Syro streamのサンプルデータを1フレーム分づつ取得します。

　　サンプル取得関数を呼びます。

　　SyroStatus SyroVolcaSample_GetSample(
　　　　SyroHandle Handle, 
　　　　int16_t *pLeft, 
　　　　int16_t *pRight
　　);
　　
　　引数の内容は、以下のようになります。
　　　　SyroHandle Handle　　　　　[in]  開始時に取得したハンドルを指定します。
　　　　int16_t *pLeft 　　　　　　[out] Lチャンネル出力用のサンプルデータを取得するポインタを指定します。
　　　　int16_t *pRight　　　　　　[out] Rチャンネル出力用のサンプルデータを取得するポインタを指定します。

　　取得したサンプルは、WAVファイル等に出力するか、そのままオーディオデバイスに出力してください。

　　この取得を、開始時にpNumOfSyroFrameに得られた数だけ繰り返します。


3.4 呼び出しの手順について(d)
　d.変換終了の関数を呼びます。

　　全てのサンプルの取得が完了したら、または、途中で中断したい場合は、
　　
　　SyroStatus SyroVolcaSample_End(SyroHandle Handle)
　　
　　関数を呼んでください。Handleには、開始時に取得したハンドルを指定します。


3.A 各関数の戻り値 (SyroStatus) について。

　　関数が成功した場合は、Status_Success が返りますが、何らかのエラーが
　　出た場合は、以下の値が返ります。

	Status_IllegalDataType　　規定外のデータ形式が指定されました。
	Status_IllegalData　　　　指定されたデータの内容が異常です。
	Status_IllegalParameter   指定されたデータの数が異常です。
	Status_OutOfRange_Number　指定されたサンプルまたはパターンの番号が範囲外です。
　　　　　　　　　　　　　　　　　(サンプル:0~99, パターン:0~9 の範囲で指定可能です)
	Status_OutOfRange_Quality 指定されたビット数が範囲外です(8~16の範囲で指定可能です)
	Status_NotEnoughMemory    作業のためのメモリが確保できません。
	
	Status_InvalidHandle　　　無効なハンドルが指定されました。
	Status_NoData　　　　　　 データ取得の際、既にSyroStreamデータ変換が完了しています。


----------------------------------------------------------------------
4. Exampleを使って syro streamを作成する。

　exampleフォルダには、volca sample SDKを使用して、syro streamをWAV形式で
　生成するためのサンプルソースがあります。

　このサンプルソースをビルドするためのVC2010用プロジェクト又はMakefileは、projectフォルダ内にあります。
　IDE等でプロジェクトを作成する場合は、exampleフォルダとsyroフォルダにあるＣソースを
　読み込んで、ビルドしてください。

　生成された実行ファイルはコンソールで使用しますが、その書式について説明します。
　コンソールで、以下のように入力します。

  >korg_syro_volcasample_example "TargetFile.wav" "SourceFile1" "SourceFile2" ......

　korg_syro_volcasample_example
　　サンプルの実行ファイル名(実行ファイル)です。
　　この名前はSDK内のソースファイル名と同一と仮定したものですが、
　　ビルド環境での指定によっては異なる名前になっているかも知れません。

　TargetFile.wav
　　生成されるsyrostreamのファイル名を指定します。
　　ファイルはWAV形式で出力されます。拡張子(.wav)まで指定してください。
　　ファイル名にスペースを入れる場合は、必ず"" (ダブルクォーテーション)で括ってください。

　SourceFile
　　変換したいsyrodataのファイルを指定します。
　　ここで種類や番号、ファイル名を一括して指定します。
　　その記述について説明します。
　　上記同様、ファイル名にスペースがある場合は、必ず"" (ダブルクォーテーション)で括ってください。

　　"x17c12:filename"
     TT~TT~T~~~~T~~~
     || || |    +------- 変換するファイル名を指定します。
 　　|| || |             種別に消去(e)を指定した場合は必要ありません。
     || || +------------ ファイル名との区切りとして:を記述してください 。
     || ||               ファイル名が不要な場合でもこれは必要です。
     || |+-------------- サンプル変換時のみの記述です。圧縮するビット数を8-16で指定してください。
     || |                省略した場合は16ビットになります。
     || +--------------- サンプル変換時のみの記述になります。
     ||                  転送時の圧縮を意味しますが、圧縮するデメリットはないので原則つけてください。
     ||
     |+----------------- サンプル番号、または、パターン番号を指定します。
     |                   サンプルの場合は0~99, パターンの場合は1~10を指定します。
     |                   全サンプルの変換時は指定しないでください。
     +------------------ 送信するファイルの種類を指定します。
                         s:sample
                         e:erase sample
                         p:pattern
                         a:all sample
　　　　　　　　　　　　をそれぞれ意味します。
                        sampleを指定する場合「16ビットまたは24ビット」「モノラルまたはステレオ」の
　　　　　　　　　　　　WAVファイルに対応しています。

　　(記述例)
　　　"s20c:kick.wav"     サンプルファイルkick.wavを、volca sample本体の20番に転送するための
　　　　　　　　　　　　　SyroStremに変換します。
　　　"s57c12:snare.wav"　サンプルファイルsnare.wavを、volca sample本体の57番に転送するための
　　　　　　　　　　　　　SyroStremに変換します。また、12Bitに落として転送時間を短くします
　　　"e27:"              volca sample本体の27番のサンプルを削除するSyroStreamを生成します。
　　　"p01:pattern1.dat"  パターンデータ pattern1.dat を、volca sample本体の１番に転送するための
　　　　　　　　　　　　　SyroStremに変換します。
　　　"ac:volcasample_preset.alldata"  全サンプルデータ volcasample_preset.alldata を転送するための
　　　　　　　　　　　　　SyroStremaに変換します。

　　これらの指定を複数並べて書くこともできます。

----------------------------------------------------------------------
5. パターンデータの構造

　SYRO for volca sampleではパターンデータも送ることが出来ますが、
　volca sampleからパターンデータを受信する方法がないため、送信する
　パターンデータを作成する必要があります。
　ここでは、パターンデータの構造について簡単に説明します。

　パターンデータの構造は、volcasample_pattern.hファイル内の、
　構造体VolcaSample_Pattern_Data で定義されています、
　以下に、それぞれのメンバの内容を説明します。
　パターンデータへの16Bit及び32Bit変数の格納は、LittleEndian形式で行ってください。
　メンバ名が、Reserved または Padding とあるものは意味を持ちませんので説明を省略します。

　uint32_t Header;
　uint16_t DevCode;
　　volca sampleのパターンであることを識別するためのフィールドです。
　　それぞれ、0x54535450、0x33b8を設定してください。
　　(VOLCASAMPLE_PATTERN_HEADER、VOLCASAMPLE_PATTERN_DEVCODEで定義されています)

　uint16_t ActiveStep;
　　Active Step のOn/Offをbitmapで示します。
　　ステップ 1~16がBit0~15に対応しており、0=Off,1=Onになります。
　　初期状態では0xffff(全部ON)にします。
　　製品仕様上、0を設定することはできません。

　VolcaSample_Part_Data Part[VOLCASAMPLE_NUM_OF_PART];
　　各パートのデータで、パート1~10が Part[0]~Part[9]に対応します。
　　内容は後述します。

　uint32_t Footer;　
　　volca sampleのパターンであることを識別するためのフィールドです。
　　0x44455450 を設定してください。
　　(VOLCASAMPLE_PATTERN_FOOTER で定義されています)


　各パートのデータは、 VolcaSample_Part_Data構造体で定義されています。
　以下に、それぞれのメンバの内容を説明します。

　uint16_t SampleNum;
　　パートで使用するサンプル番号を指定します。
　　0 ~ 99が指定可能です。

　uint16_t StepOn;　
　　各ステップのOn/Offをbitmapで示します。
　　ステップ 1~16がBit0~15に対応しており、0=Off,1=Onになります。
　　
　uint16_t Accent;
　　このパラメータはvolca sampleで操作できないので、0を設定してください。

　uint8_t Level;　
　　このパラメータはvolca sampleで操作できません。最大を示す127を設定してください。

　uint8_t Param[VOLCASAMPLE_NUM_OF_PARAM];
　　ノブパラメータ位置の状態を示します。
　　配列の番号とパラメータの関係性は以下のようになります(カッコ内は初期値です)
　　0 : LEVEL           0~127 (127)
    1 : PAN             1~127, 64=Center (64)
    2 : SPEED           40~88, 64=Center (64) *Func+Speedノブ操作時=Note単位
                        129~255, 192=Center   *Speedノブ操作時
    3 : AMP EG ATTACK   0~127 (0)
    4 : AMP EG DECAY    0~127 (127)
    5 : PITCH EG INT    1~127, 64=Center (64)
    6 : PITCH EG ATTACK 0~127 (0)
    7 : PITCH EG DECAY  0~127 (127)
    8 : START POINT     0~127 (0)
    9 : LENGTH          0~127 (127)
    10: HI CUT          0~127 (127)

　　これらの番号は、VOLCASAMPLE_PARAM_xxxxでも定義されています。

　uint8_t FuncMemoryPart;
　　パートの各種On/Off設定をbitmapで示します。
　　各bitとパラメータの関係性は以下のようになります。
　　bit0 : Motion On/Off
　　bit1 : Loop On/Off
　　bit2 : Reverb On/Off
　　bit3 : Reverse On/Off
　　bit4 : Mute On/Off (1=Onが音が鳴る状態です)

　　これらの番号は、VOLCASAMPLE_FUNC_BIT_xxxxでも定義されています。

　uint8_t Motion[VOLCASAMPLE_NUM_OF_MOTION][VOLCASAMPLE_NUM_OF_STEP];
　　モーションデータを示します。
　　１つめの[]でモーションデータの種類、２つめの[]でステップ番号を示します。
　　１つめの[]の番号とモーションの種類の関係性は以下のようになります。

　　0 : LEVEL (始点)
　　1 : LEVEL (終点)
　　2 : PAN (始点)
　　3 : PAN (終点)
　　4 : SPEED (始点)
　　5 : SPEED (終点)
      LEVEL,PAN,SPEEDの３つのパラメータについては、１ステップ内に
　　　始点・終点の２カ所の値を記録してあり、ステップ再生時になめらかに
　　　なるよう補間する動きをしています。
　　6 : AMP EG ATTACK
　　7 : AMP EG DECAY
　　8 : PITCH EG INT
　　9 : PITCH EG ATTACK
　　10: PITCH EG DECAY
　　11: SATRT POINT
　　12: LENGTH
　　13: HI CUT

　　モーションとして設定する値は、
　　・SPEED以外  ノブパラメータ+128
　　・SPEED      ノブパラメータそのまま
　　になります。いずれの場合も、0=モーションデータなし、の意味になります。

　volcasample_pattern.cには、パターンデータを初期化する関数、
　void VolcaSample_Pattern_Init(VolcaSample_Pattern_Data *pattern_data)
　を用意しています。
　上記パラメータの識別データの設定や各パラメータの初期値設定を行いますので
　必要に応じて呼び出してください。


----------------------------------------------------------------------
6. volca sampleへの転送方法について。

　volca sampleのシステムバージョンは Ver 1.22 以降にアップデートして使ってください。
　アップデーターについては、コルグ・ホームページ
　http://www.korg.com/
　を参照してください。
　システムバージョンは、[REC]ボタンを押しながら起動することで確認できます。
　[REC]ボタンを押しながら起動すると、以下の表示を繰り返します。
　 x.yy → システムバージョンです。
  Px.yy → パネルバージョンです。
  Sx.yy → サンプルバージョンです。

   x.yyの部分は、バージョン番号が表示されます

　このうち、システムバージョンが1.00と表示されている場合は、
　アップデートが必要になります。
　パネル及びサンプルのバージョンはSyroを使う上では関係ありません。


　PC等の再生機のAudio Out端子と、volca sampleのSYNC IN端子をステレオケーブルで
　接続し、syroで生成されたオーディオデータを再生します。
　この時、再生機側の音量は最大にしてください。

　!!注意!!
　絶対にSyroのオーディオデータをスピーカーで鳴らしたりヘッドホンで聞いたりしないでください。
　機器の故障や耳を傷める原因になります。

　volca sampleは信号を検出したら、ディスプレイに [dAtA] [種別] を交互に
　表示し、受信モードに切り替わります。
　受信が完了したら、 [End ]と表示されます。

　エラーが出た場合は、[Err ] [種別] を交互に表示し、ノブが点滅します。

　[End ]または[Err ]が表示されている状態で、[FUNC]を押すと、通常動作に戻ります。


6.1 受信中のデータ表示について。
　受信中は[dAtA][種別]を交互に表示しますが、この種別について列記します。

　S.000 ~ S.099  : サンプル0~99を受信中です。
　P.001 ~ P.010  : パターン1~10を受信中です。
　E.000 ~ E.099  : サンプル0~99を消去中です。
　ALL            : 全サンプルを受信中です。

　パターンの受信は、volca sampleの保存メモリに対して行われます。
　パターンを受信したら、そのパターンを本体操作で読み込んでください。
　サンプル転送中にエラーが出た場合、その番号のサンプルが消えてしまうことがあります。
　また、全サンプルの転送中にエラーが出た場合、サンプルが１つもない状態になります。
　これらの場合は、再度サンプルを転送しなおしてください。

6.2 エラー表示について。
　エラーが出た場合は、以下の点について確認してください。
　・再生機とvolca sampleの接続は正しく行われているか？
　　ケーブルは必ずステレオケーブルを使ってください。
　・再生機の音量は最大になっているか？
　・[Err ][tyPE] が出る場合は、volca sampleのシステムバージョンが最新かどうか確認してください。
　・[Err ][FuLL] が出る場合は、volca sampleのサンプルメモリの空きが不足しています。
　　この場合は。volca sampleの操作でサンプルを削除してください。
　・[Err ][btLo] が出る場合は、電池の容量を確認してください。

　その他、特定のサンプルでエラーを繰り返す等の場合は、
　https://github.com/korginc/volcasample/issues
　で問い合わせて頂くか、同様の質問を検索してみてください。


