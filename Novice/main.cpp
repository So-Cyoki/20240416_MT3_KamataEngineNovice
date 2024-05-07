#include <Novice.h>
#include <cassert>
#include <cmath>
#include <imgui.h>

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
const float kWindowWidth = 1280;
const float kWindwoHeight = 720;

// アフィン変換
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation);
// 掛け算
Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);
// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// クロス積(外積)
Vector3 Cross(const Vector3& v1, const Vector3& v2);
// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// 行列の値をスクリーンに出す
const int kColumnWidth = 60;
const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label);
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

void DrawGrid(const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, int(kWindowWidth), int(kWindwoHeight));

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// 自分の変数
	Vector3 cameraPostion = {0, 5, -10};
	Vector3 cameraRotate = {0.45f, 0, 0};
	Vector3 gridPostion{0, 0, 0};

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

		// DebugText
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraPostion.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::End();

		// レンダリングパイプライン計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1, 1, 1}, cameraRotate, cameraPostion);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 viewprotMatrix = MakeViewportMatrix(0, 0, kWindowWidth, kWindwoHeight, 0, 1);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, kWindowWidth / kWindwoHeight, 0.1f, 100);

		Matrix4x4 worldMatrix_grid = MakeAffineMatrix({1, 1, 1}, {0, 0, 0}, gridPostion);
		Matrix4x4 worldViewProjectionMatrix_grid = Multiply(worldMatrix_grid, Multiply(viewMatrix, projectionMatrix));

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix_grid, viewprotMatrix);

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

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result = {0};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.f * matrix.m[3][3];

	assert(w != 0);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float detA =
	    m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	float adjA[4][4] = {0};
	adjA[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	             m.m[1][1] * m.m[2][3] * m.m[3][2];
	adjA[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	             m.m[0][1] * m.m[2][3] * m.m[3][2];
	adjA[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	             m.m[0][1] * m.m[1][3] * m.m[3][2];
	adjA[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	             m.m[0][1] * m.m[1][3] * m.m[2][2];
	adjA[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	             m.m[1][0] * m.m[2][3] * m.m[3][2];
	adjA[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	             m.m[0][0] * m.m[2][3] * m.m[3][2];
	adjA[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	             m.m[0][0] * m.m[1][3] * m.m[3][2];
	adjA[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	             m.m[0][0] * m.m[1][3] * m.m[2][2];
	adjA[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	             m.m[1][0] * m.m[2][3] * m.m[3][1];
	adjA[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	             m.m[0][0] * m.m[2][3] * m.m[3][1];
	adjA[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	             m.m[0][0] * m.m[1][3] * m.m[3][1];
	adjA[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	             m.m[0][0] * m.m[1][3] * m.m[2][1];
	adjA[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	             m.m[1][0] * m.m[2][2] * m.m[3][1];
	adjA[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	             m.m[0][0] * m.m[2][2] * m.m[3][1];
	adjA[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	             m.m[0][0] * m.m[1][2] * m.m[3][1];
	adjA[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	             m.m[0][0] * m.m[1][2] * m.m[2][1];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 1 / detA * adjA[i][j];
		}
	}
	return result;
}

Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {0};

	result.m[0][0] = 1 / aspectRatio * cosf(fovY / 2) / sinf(fovY / 2);
	result.m[1][1] = cosf(fovY / 2) / sinf(fovY / 2);
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[2][3] = 1;
	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {0};
	result.m[0][0] = 2 / (right - left);
	result.m[1][1] = 2 / (top - bottom);
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {0};
	result.m[0][0] = width / 2;
	result.m[1][1] = -height / 2;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + width / 2;
	result.m[3][1] = top + height / 2;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

void MatrixScreenPrintf(int x, int y, const Matrix4x4 matrix, const char* label) {
	Novice::ScreenPrintf(x, y, label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void DrawGrid(const Matrix4x4& viewProjectionM, const Matrix4x4& viewprotM) {
	const float kGridHalfWidth = 2.f;                                    // Girdの半分の幅
	const uint32_t kSubdivision = 10;                                    // 分割数
	const float kGridEvery = (kGridHalfWidth * 2) / float(kSubdivision); // 一つ分の長さ

	// 線の座標を記録する
	Vector3 horizontalStartPos_local[kSubdivision + 1]{};
	Vector3 horizontalEndPos_local[kSubdivision + 1]{};
	Vector3 verticalStartPos_local[kSubdivision + 1]{};
	Vector3 verticalEndPos_local[kSubdivision + 1]{};

	Vector3 horizontalStartPos_screen[kSubdivision + 1]{};
	Vector3 horizontalEndPos_screen[kSubdivision + 1]{};
	Vector3 verticalStartPos_screen[kSubdivision + 1]{};
	Vector3 verticalEndPos_screen[kSubdivision + 1]{};

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; xIndex++) {
		// ローカル座標を計算する
		horizontalStartPos_local[xIndex].x = -kGridHalfWidth;
		horizontalEndPos_local[xIndex].x = kGridHalfWidth;
		horizontalStartPos_local[xIndex].z = kGridHalfWidth - xIndex * kGridEvery;
		horizontalEndPos_local[xIndex].z = kGridHalfWidth - xIndex * kGridEvery;
		// スクリーン座標に変換する
		Vector3 ndcStart = Transform(horizontalStartPos_local[xIndex], viewProjectionM);
		horizontalStartPos_screen[xIndex] = Transform(ndcStart, viewprotM);
		Vector3 ndcEnd = Transform(horizontalEndPos_local[xIndex], viewProjectionM);
		horizontalEndPos_screen[xIndex] = Transform(ndcEnd, viewprotM);
		// 描画
		if (xIndex != 5)
			Novice::DrawLine(
			    int(horizontalStartPos_screen[xIndex].x), int(horizontalStartPos_screen[xIndex].y), int(horizontalEndPos_screen[xIndex].x), int(horizontalEndPos_screen[xIndex].y), 0xAAAAAAFF);
		else
			Novice::DrawLine(int(horizontalStartPos_screen[xIndex].x), int(horizontalStartPos_screen[xIndex].y), int(horizontalEndPos_screen[xIndex].x), int(horizontalEndPos_screen[xIndex].y), BLACK);
	}
	// 左から右への線を順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; zIndex++) {
		// ローカル座標を計算する
		verticalStartPos_local[zIndex].x = kGridHalfWidth - zIndex * kGridEvery;
		verticalEndPos_local[zIndex].x = kGridHalfWidth - zIndex * kGridEvery;
		verticalStartPos_local[zIndex].z = -kGridHalfWidth;
		verticalEndPos_local[zIndex].z = kGridHalfWidth;
		// スクリーン座標に変換する
		Vector3 ndcStart = Transform(verticalStartPos_local[zIndex], viewProjectionM);
		verticalStartPos_screen[zIndex] = Transform(ndcStart, viewprotM);
		Vector3 ndcEnd = Transform(verticalEndPos_local[zIndex], viewProjectionM);
		verticalEndPos_screen[zIndex] = Transform(ndcEnd, viewprotM);
		// 描画
		if (zIndex != 5)
			Novice::DrawLine(int(verticalStartPos_screen[zIndex].x), int(verticalStartPos_screen[zIndex].y), int(verticalEndPos_screen[zIndex].x), int(verticalEndPos_screen[zIndex].y), 0xAAAAAAFF);
		else
			Novice::DrawLine(int(verticalStartPos_screen[zIndex].x), int(verticalStartPos_screen[zIndex].y), int(verticalEndPos_screen[zIndex].x), int(verticalEndPos_screen[zIndex].y), BLACK);
	}
}
