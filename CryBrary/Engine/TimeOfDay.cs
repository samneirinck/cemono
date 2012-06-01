using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public static class TimeOfDay
	{
		/// <summary>
		/// Controls whether Time of Day updates take effect immediately.
		/// </summary>
		public static bool ForceUpdates { get; set; }

		/// <summary>
		/// The hour value for the Time of Day system.
		/// The value is wrapped, so setting the value to 24 will reset the hour to zero.
		/// </summary>
		public static int Hour
		{
			get
			{
				return (int)Engine._GetTimeOfDay();
			}
			set
			{
				while(value >= 24)
				{
					value -= 24;
				}
				while(value < 0)
				{
					value += 24;
				}

				RawEngineTime = CreateEngineTime(value, Minute);
			}
		}

		/// <summary>
		/// The minute value for the Time of Day system.
		/// The value is wrapped, so setting the value to 60 will increment the hour and reset the minutes to zero.
		/// </summary>
		public static int Minute
		{
			get
			{
				return GetMinutes(Engine._GetTimeOfDay());
			}
			set
			{
				RawEngineTime = CreateEngineTime(Hour, value);
			}
		}

		/// <summary>
		/// Controls the speed at which the Time of Day passes.
		/// </summary>
		public static float Speed
		{
			get
			{
				return Engine._GetTimeOfDayAdvancedInfo().fAnimSpeed;
			}
			set
			{
				var info = Engine._GetTimeOfDayAdvancedInfo();
				info.fAnimSpeed = value;
				Engine._SetTimeOfDayAdvancedInfo(info);
			}
		}

		/// <summary>
		/// Gets the minute value from a CE-style time float
		/// </summary>
		/// <param name="ceTime"></param>
		/// <returns></returns>
		internal static int GetMinutes(float ceTime)
		{
			return (int)System.Math.Round((ceTime - (int)ceTime) * 60);
		}

		/// <summary>
		/// Gets the hour value from a CE-style time float
		/// </summary>
		/// <param name="ceTime"></param>
		/// <returns></returns>
		internal static int GetHours(float ceTime)
		{
			return (int)ceTime;
		}

		/// <summary>
		/// Creates a CE-style time from a given number of hours and minutes
		/// </summary>
		/// <param name="hours"></param>
		/// <param name="mins"></param>
		/// <returns></returns>
		internal static float CreateEngineTime(int hours, int mins)
		{
			return hours + ((float)mins / 60);
		}

		/// <summary>
		/// Convenient accessor for the raw time value
		/// </summary>
		internal static float RawEngineTime
		{
			get
			{
				return Engine._GetTimeOfDay();
			}
			set
			{
				Engine._SetTimeOfDay(value, ForceUpdates);
			}
		}

		internal struct AdvancedInfo
		{
			public float fStartTime;
			public float fEndTime;
			public float fAnimSpeed;
		}

		// TODO: Make sure people can't send color values to float parameters and vice versa.
		#region SetVariableValue methods
		public static void SetVariableValue(SkyParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(SkyParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(FogParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(FogParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(SkyLightParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(SkyLightParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(NightSkyParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(NightSkyParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(CloudShadingParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(CloudShadingParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(SunRaysEffectParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(SunRaysEffectParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(ColorGradingParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(ColorGradingParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(ShadowParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(ShadowParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}

		public static void SetVariableValue(HDRParams param, float value)
		{
			Engine._SetTimeOfDayVariableValue((int)param, value);
		}

		public static void SetVariableValue(HDRParams param, Vec3 value)
		{
			Engine._SetTimeOfDayVariableValueColor((int)param, value);
		}
		#endregion

		#region Time of Day parameters
		public enum SkyParams
		{
			SkyBrightening = 1,
			GlobalIlluminationMultiplier = 3,

			SunColor,
			SunColorMultiplier,
			SunSpecularMultiplier,

			SkyColor,
			SkyColorMultiplier,

			AmbientGroundColor,
			AmbientGroundColorMultiplier,

			AmbientMinHeight,
			AmbientMaxHeight,

			SkyboxMultiplier = 70,
		}

		public enum FogParams
		{
			ColorBottom = 14,
			ColorBottomMultiplier,
			HeightBottom,
			DensityBottom,

			ColorTop,
			ColorTopMultiplier,
			HeightTop,
			DensityTop,

			ColorHeightOffset,

			ColorRadial,
			ColorRadialMultiplier,
			RadialSize,
			RadialLobe,

			FinalDensityClamp,

			GlobalDensity,
			RampStart,
			RampEnd,
			RampInfluence,

			OceanFogColor = 67,
			OceanFogColorMultiplier,
			OceanFogDesnity,
		}

		public enum SkyLightParams
		{
			SunIntensity = 32,
			SunIntensityMultiplier,

			MieScattering,
			RayleighScattering,
			SunAntisotropyFactor,

			WaveLengthR,
			WaveLengthG,
			WaveLengthB,
		}

		public enum NightSkyParams
		{
			HorizonColor = 40,
			HorizonColorMultiplier,
			ZenithColor,
			ZenithColorMultiplier,
			ZenithShift,

			StarIntensity,

			MoonColor,
			MoonColorMultiplier,

			MoonInnerCoronaColor,
			MoonInnerCoronaColorMultiplier,
			MoonInnerCoronaScale,

			MoonOuterCoronaColor,
			MoonOuterCoronaColorMultiplier,
			MoonOuterCoronaScale,
		}

		public enum CloudShadingParams
		{
			SunlightMultiplier = 54,
			SkylightMultiplier,

			SunlightCustomColor,
			SunlightCustomColorMultiplier,
			SunlightCustomColorInfluence,

			SkylightCustomColor,
			SkylightCustomColorMultiplier,
			SkylightCustomColorInfluence,
		}

		public enum SunRaysEffectParams
		{
			SunshaftVisibility = 62,
			SunRayVisibility,
			SunRayAttenuation,
			SunRaySunColorInfluence,
			SunRayCustomColor,
		}

		public enum ColorGradingParams
		{
			HDR_ColorSaturation = 77,
			HDR_ColorContrast,
			HDR_ColorBalance,

			ColorSaturation,
			ColorContrast,
			ColorBrightness,

			MinInput,
			Gamme,
			MaxInput,
			MinOutput,
			MaxOutput,

			SelectiveColor_Color,
			SelectiveColor_Cyans,
			SelectiveColor_Magnetas,
			SelectiveColor_Yellows,
			SelectiveColor_Blacks,

			Filters_Grain,
			Filters_Sharpening,
			Filters_PhotofilterColor,
			Filters_PhotofilterDensity,

			DepthOfField_FocusRange,
			DepthOfField_BlurAmount
		}

		public enum ShadowParams
		{
			Cascade_0_Bias = 99,
			Cascade_0_SlopeBias,
			Cascade_1_Bias,
			Cascade_1_SlopeBias,
			Cascade_2_Bias,
			Cascade_2_SlopeBias,
			Cascade_3_Bias,
			Cascade_3_SlopeBias,
		}

		public enum HDRParams
		{
			DynamicPowerFactor = 0,

			FilmCurve_ShoulderScale = 71,
			FilmCurve_LinearScale,
			FilmCurve_ToeScale,
			FilmCurve_WhitePoint,

			BlueShift,
			BlueShiftThreshold,
		}
		#endregion
	}
}
