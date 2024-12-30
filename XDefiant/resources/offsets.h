#pragma once
#include <iostream>
#include "../core/sdk/math/Vector.hpp"

struct vmatrix_t {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		}; float m[4][4];
	};
};

class ViewData
{
public:
	/* 0x0 */ char pad_0[0x10];
	/* 0x10 */ vmatrix_t Matrix;
	/* 0x50 */ char pad_1[0x40];
	/* 0x90 */ Vector CameraDir; // Confirmed, writing doesnt effect it!
	/* 0x9C */ char pad_2[0x4];
	/* 0xA0 */ Vector CameraPos;
	/* 0xAC */ char pad_3[0x1C];
	/* 0xBC */ Vector FOV;
};

struct WeaponComponent {
	uint64_t* vtable; // 0x0
private: char pad_8[0xc8]; public:
	Vector currentPosition; // 0xd0
private: char pad_dc[0x4c]; public:
	uint32_t currentAmmo; // 0x128
	uint8_t isCrouched; // 0x12c
private: char pad_12d[0xb]; public:
	float isScoped; // 0x138
}; // Size: 0x13c