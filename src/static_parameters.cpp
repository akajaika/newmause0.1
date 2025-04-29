#define IO_OUT	(1)				// PFCのInput/Output設定を1に設定すると出力になる
#define IO_IN	(0)				// PFCのInput/Output設定を0に設定すると入力になる

#define BATT_MAX		2552		// 12.4V
#define BATT_MIN		2034		// 10V

#define SW_OFF	(1)				// スイッチがOFFの状態の値
#define SW_ON	(0)				// スイッチがONの状態の値

#define CHATTERING_WAIT	(50)			// チャタリング防止用の待機時間

#define INC_FREQ	(2000)			// モータ加速時の周波数
#define DEC_FREQ	(1000)			// モータ減速時の周波数

#define MOT_R_FORWARD	(0)			// モータ右回転方向CW/CCW信号をLOWにすると前進
#define MOT_R_BACK	(1)			// モータ右回転方向CW/CCW信号をHIGHにすると後退
#define MOT_L_FORWARD	(1)			// モータ左回転方向CW/CCW信号をLOWにすると前進
#define MOT_L_BACK	(0)			// モータ左回転方向CW/CCW信号をHIGHにすると後退

//#define MIN_SPEED	(0.3)			// 最小速度。浮動小数点数を使用する場合、MTUの設定により18mm/sが最小値となるため、目標値は30mm/s以上に設定

#define PI (3.141592653589793)			// 円周率

// 方向
#define RIGHT	(0)
#define LEFT	(1)
#define FRONT	(2)
#define REAR	(3)

// 終了条件フラグ
#define END_FAST	(4)

#define HALF_SECTION	(45)			// 半区画の長さ
#define SECTION			(90)		// 1区画の長さ

#define MAZESIZE_X		(16)		// 迷路の横幅 (MAZESIZE_X * MAZESIZE_Y)
#define MAZESIZE_Y		(16)		// 迷路の縦幅 (MAZESIZE_X * MAZESIZE_Y)

#define UNKNOWN	2				// 壁があるかどうかわからない場合の値
#define NOWALL	0				// 壁がない場合の値
#define WALL	1				// 壁がある場合の値
#define VWALL	3				// 仮想壁の値 (デバッグ用)

#define STRAIGHT_MODE	0			// 直進モード
#define TURN_MODE	1			// 旋回モード
#define SLA_MODE	2			// スラロームモード
#define NON_CON_MODE	3			// 無制御モード
#define TEST_MODE	4			// テストモード (デバッグ用のモータ動作確認モード)
#define F_WALL_MODE	5
#define SLALOM_MODE	6	

#define MASK_SEARCH		0x01		// 探索走行用マスク値。現在の値とAND演算してNOWALLでなければ探索済み
#define MASK_SECOND		0x03		// 二次走行用マスク値。現在の値とAND演算してNOWALLでなければ探索済み
