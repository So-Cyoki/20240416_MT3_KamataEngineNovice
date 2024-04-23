#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "GC2A_07_ソウ_チョウキ_MT3";

// 自分の型
struct Vector3 {
	float x;
	float y;
	float z;
};
struct Matrix4x4 {
	float m[4][4];
};

Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);

// 画面にVector3とMatrix4x4を表示する
static const int kColumnWidth = 60;
static const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// 自分の変数
	Vector3 rotate = {0.4f, 1.43f, -0.8f};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(10, 10, rotateXMatrix, "RotateXMatrix");
		MatrixScreenPrintf(10, 10 + kRowHeight * 5, rotateYMatrix, "RotateYMatrix");
		MatrixScreenPrintf(10, 10 + kRowHeight * 5 * 2, rotateZMatrix, "RotateZMatrix");
		MatrixScreenPrintf(10, 10 + kRowHeight * 5 * 3, rotateXYZMatrix, "RotateXYZMatrix");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 mRotationX = {0};
	mRotationX.m[1][1] = cosf(radian);
	mRotationX.m[1][2] = sinf(radian);
	mRotationX.m[2][1] = -sinf(radian);
	mRotationX.m[2][2] = cosf(radian);
	mRotationX.m[0][0] = mRotationX.m[3][3] = 1;
	return mRotationX;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 mRotationY = {0};
	mRotationY.m[0][0] = cosf(radian);
	mRotationY.m[2][0] = sinf(radian);
	mRotationY.m[0][2] = -sinf(radian);
	mRotationY.m[2][2] = cosf(radian);
	mRotationY.m[1][1] = mRotationY.m[3][3] = 1;
	return mRotationY;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 mRotationZ = {0};
	mRotationZ.m[0][0] = cosf(radian);
	mRotationZ.m[0][1] = sinf(radian);
	mRotationZ.m[1][0] = -sinf(radian);
	mRotationZ.m[1][1] = cosf(radian);
	mRotationZ.m[2][2] = mRotationZ.m[3][3] = 1;
	return mRotationZ;
}

Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label) {
	Novice::ScreenPrintf(x, y, label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}
