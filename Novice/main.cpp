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

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation);
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
	Vector3 scale = {1.2f, 0.79f, -2.1f};
	Vector3 rotate = {0.4f, 1.43f, -0.8f};
	Vector3 translate = {2.7f, -4.15f, 1.57f};

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

		Matrix4x4 worldMatrix = MakeAffineMatrix(scale, rotate, translate);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(10, 10, worldMatrix, "WorldMatrix");

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

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation) {
	// Scale
	Matrix4x4 mScale = {0};
	mScale.m[0][0] = scale.x;
	mScale.m[1][1] = scale.y;
	mScale.m[2][2] = scale.z;
	mScale.m[3][3] = 1;
	// Rotation
	Matrix4x4 mRotationZ = {0};
	mRotationZ.m[0][0] = cosf(rotation.z);
	mRotationZ.m[0][1] = sinf(rotation.z);
	mRotationZ.m[1][0] = -sinf(rotation.z);
	mRotationZ.m[1][1] = cosf(rotation.z);
	mRotationZ.m[2][2] = mRotationZ.m[3][3] = 1;
	Matrix4x4 mRotationX = {0};
	mRotationX.m[1][1] = cosf(rotation.x);
	mRotationX.m[1][2] = sinf(rotation.x);
	mRotationX.m[2][1] = -sinf(rotation.x);
	mRotationX.m[2][2] = cosf(rotation.x);
	mRotationX.m[0][0] = mRotationX.m[3][3] = 1;
	Matrix4x4 mRotationY = {0};
	mRotationY.m[0][0] = cosf(rotation.y);
	mRotationY.m[2][0] = sinf(rotation.y);
	mRotationY.m[0][2] = -sinf(rotation.y);
	mRotationY.m[2][2] = cosf(rotation.y);
	mRotationY.m[1][1] = mRotationY.m[3][3] = 1;
	Matrix4x4 mRotation = Multiply(mRotationX, Multiply(mRotationY, mRotationZ));
	// Translation
	Matrix4x4 mTranslation = {0};
	mTranslation.m[0][0] = mTranslation.m[1][1] = mTranslation.m[2][2] = mTranslation.m[3][3] = 1;
	mTranslation.m[3][0] = translation.x;
	mTranslation.m[3][1] = translation.y;
	mTranslation.m[3][2] = translation.z;

	return Multiply(mScale, Multiply(mRotation, mTranslation));
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
