#pragma once 

#define EXPORTED __declspec ( dllexport )
#define NAKED __declspec ( naked )

#ifndef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else 
#define ALWAYS_INLINE __attribute__ ( ( always_inline ) ) inline
#endif

namespace __sdk_constant_holder { template< auto _value > inline constexpr auto k_value = _value; }

#define CONSTANT( constant ) __sdk_constant_holder::k_value< constant >

#define LOG(...) \
 std::printf( OBFUSCATE_STR( "[flaw-fn-build] " ), __FUNCTION__ ); \
 std::printf( __VA_ARGS__ )

#define ENUM_UNDERLYING_OPERATOR( enum_t ) \
    ALWAYS_INLINE constexpr auto operator -( const enum_t value ) { \
        return static_cast< std::underlying_type_t< enum_t > >( value ); \
    } \

#define ENUM_BIT_OPERATOR( enum_t, op, ret_underlying ) \
    template < typename _value_t > \
        requires std::_Is_any_of_v< _value_t, enum_t, std::underlying_type_t< enum_t > > \
    ALWAYS_INLINE constexpr auto operator op( const enum_t lhs, const _value_t rhs ) { \
        using underlying_t = std::underlying_type_t< enum_t >; \
        \
        using ret_t = std::conditional_t< ret_underlying, underlying_t, enum_t >; \
        \
        return static_cast< ret_t >( \
            static_cast< underlying_t >( lhs ) op static_cast< underlying_t >( rhs ) \
        ); \
    } \
    \
    ALWAYS_INLINE constexpr auto operator op( const std::underlying_type_t< enum_t > lhs, const enum_t rhs ) { \
        using underlying_t = std::underlying_type_t< enum_t >; \
        \
        using ret_t = std::conditional_t< ret_underlying, underlying_t, enum_t >; \
        \
        return static_cast< ret_t >( \
            static_cast< underlying_t >( lhs ) op static_cast< underlying_t >( rhs ) \
        ); \
    } \
    \
    template < typename _value_t > \
        requires std::_Is_any_of_v< _value_t, enum_t, std::underlying_type_t< enum_t > > \
    ALWAYS_INLINE auto& operator op##=( enum_t& lhs, const _value_t rhs ) { \
        using underlying_t = std::underlying_type_t< enum_t >; \
        \
        using ret_t = std::conditional_t< ret_underlying, underlying_t, enum_t >; \
        \
        return reinterpret_cast< ret_t& >( \
            reinterpret_cast< underlying_t& >( lhs ) op##= static_cast< underlying_t >( rhs ) \
        ); \
    } \

#define ENUM_BIT_OPERATORS( enum_t, ret_underlying ) \
    ENUM_BIT_OPERATOR( enum_t, |, ret_underlying ) \
    \
    ENUM_BIT_OPERATOR( enum_t, &, ret_underlying ) \
    \
    ENUM_BIT_OPERATOR( enum_t, ^, ret_underlying ) \
    \
    ALWAYS_INLINE constexpr auto operator ~( const enum_t value ) { \
        using underlying_t = std::underlying_type_t< enum_t >; \
        \
        using ret_t = std::conditional_t< ret_underlying, underlying_t, enum_t >; \
        \
        return static_cast< ret_t >( ~static_cast< underlying_t >( value ) ); \
    } \

#if defined( _WIN32 ) || defined( _WIN64 )
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#endif

#include <winternl.h>
#include <ntstatus.h>
#include <tlhelp32.h>

#include <iostream>
#include <dwmapi.h>
#include <string_view>
#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include <type_traits>

#include <optional>
#include <future>
#include <stdio.h>
#include <array>
#include <format>
#include <ranges>
#include <numbers>
#include <chrono>
#include <sddl.h>

