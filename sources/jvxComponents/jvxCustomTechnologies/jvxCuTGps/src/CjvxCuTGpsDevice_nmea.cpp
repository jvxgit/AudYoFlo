#include "CjvxCuTGpsDevice.h"

//#define JVX_GPS_DEBUG_GPGLL

#ifdef JVX_GPS_DEBUG_GPVTG
#define GPVTG_MAX_NUM 9
static const char* gpvtg[GPVTG_MAX_NUM] =
{
	"Track made good",
	"Fixed text 'T' indicates that track made good is relative to true north",
	"not used",
	"not used",
	"Speed over ground in knots",
	"Fixed text 'N' indicates that speed over ground in in knots",
	"Speed over ground in kilometers / hour",
	"Fixed text 'K' indicates that speed over ground is in kilometers / hour",
	"Checksum"
};
#endif

#ifdef JVX_GPS_DEBUG_GPGLL
#define GPGLL_MAX_NUM 5
static const char* gpgll[GPGLL_MAX_NUM] =
{
	"Current latitude",
	"North / South",
	"Current longitude",
	"East / West",
	"checksum"
};
#endif

#ifdef JVX_GPS_DEBUG_GPGGA
#define GPGGA_MAX_NUM 15
static const char* gpgga[GPGGA_MAX_NUM] =
{
	"UTC of Position",
	"Latitude",
	"N or S",
	"Longitude",
	"E or W",
	"GPS quality indicator(0 = invalid; 1 = GPS fix; 2 = Diff.GPS fix)",
	"Number of satellites in use[not those in view]",
	"Horizontal dilution of position",
	"Antenna altitude above / below mean sea level(geoid)",
	"Meters(Antenna height unit)",
	"Geoidal separation(Diff.between WGS - 84 earth ellipsoid and mean sea level. -= geoid is below WGS - 84 ellipsoid)",
	"Meters(Units of geoidal separation)",
	"Age in seconds since last update from diff.reference station",
	"Diff.reference station ID#",
	"Checksum"
};
#endif

void 
CjvxCuTGpsDevice::convert_nmea_gpvtg(const std::string& token)
{
	jvxSize i;
	jvxBool err = false;
	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

	/*
		Track Made Good and Ground Speed.
		eg1. $GPVTG,360.0,T,348.7,M,000.0,N,000.0,K*43
		eg2. $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K

           054.7,T      True track made good
           034.4,M      Magnetic track made good
           005.5,N      Ground speed, knots
           010.2,K      Ground speed, Kilometers per hour

		eg3. $GPVTG,t,T,,,s.ss,N,s.ss,K*hh
		1    = Track made good
		2    = Fixed text 'T' indicates that track made good is relative to true north
		3    = not used
		4    = not used
		5    = Speed over ground in knots
		6    = Fixed text 'N' indicates that speed over ground in in knots
		7    = Speed over ground in kilometers/hour
		8    = Fixed text 'K' indicates that speed over ground is in kilometers/hour
		9    = Checksum

		The actual track made good and speed relative to the ground.

		$--VTG,x.x,T,x.x,M,x.x,N,x.x,K
		x.x,T = Track, degrees True
		x.x,M = Track, degrees Magnetic
		x.x,N = Speed, knots
		x.x,K = Speed, Km/hr */

#ifdef JVX_GPS_DEBUG_GPVTG

	std::cout << "=============== GPVTG: ===============" << std::endl;
	for (i = 0; i < paramLst.size(); i++)
	{
		if (i < GPVTG_MAX_NUM)
		{
			std::cout << gpvtg[i] << " -- " << std::flush;
		}
		std::cout << paramLst[i] << std::endl;
	}
	std::cout << "======================================" << std::endl;
#endif
	genGpsRs232_device::remote.gpvtg.speed_knots.value = 0;
	genGpsRs232_device::remote.gpvtg.speed_kph.value = 0;
	if (paramLst.size() >= 7)
	{
		genGpsRs232_device::remote.gpvtg.speed_knots.value = jvx_string2Data(paramLst[4], err);
		genGpsRs232_device::remote.gpvtg.speed_kph.value = jvx_string2Data(paramLst[6], err);
	}
}

void 
CjvxCuTGpsDevice::convert_nmea_gpgga(const std::string& token)
{
	jvxBool err = false;
	jvxSize i;

	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

	/*
	$GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	1    = UTC of Position
	2    = Latitude
	3    = N or S
	4    = Longitude
	5    = E or W
	6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
	7    = Number of satellites in use [not those in view]
	8    = Horizontal dilution of position
	9    = Antenna altitude above/below mean sea level (geoid)
	10   = Meters  (Antenna height unit)
	11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
       mean sea level.  -=geoid is below WGS-84 ellipsoid)
	12   = Meters  (Units of geoidal separation)
	13   = Age in seconds since last update from diff. reference station
	14   = Diff. reference station ID# <- ???
	15   = Checksum
	*/
#ifdef JVX_GPS_DEBUG_GPGGA
	std::cout << "=============== GPVTG: ===============" << std::endl;
	for (i = 0; i < paramLst.size(); i++)
	{
		if (i < GPGGA_MAX_NUM)
		{
			std::cout << gpgga[i] << " -- " << std::flush;
		}
		std::cout << paramLst[i] << std::endl;
	}
	std::cout << "======================================" << std::endl; 
#endif
	genGpsRs232_device::remote.gpgga.utc.value = "--:--:--";
	genGpsRs232_device::remote.gpgga.latitude.value = 0;
	jvx_bitZSet(genGpsRs232_device::remote.gpgga.north_south.value.selection(), 2);
	genGpsRs232_device::remote.gpgga.longitude.value = 0;
	jvx_bitZSet(genGpsRs232_device::remote.gpgga.east_west.value.selection(), 2);
	jvx_bitZSet(genGpsRs232_device::remote.gpgga.quality.value.selection(), 0);
	genGpsRs232_device::remote.gpgga.num_satellites.value = 0;
	genGpsRs232_device::remote.gpgga.horizontal_dilution.value = 0;
	genGpsRs232_device::remote.gpgga.altitude.value = 0;
	jvx_bitZSet(genGpsRs232_device::remote.gpgga.height_unit.value.selection(), 1);
	genGpsRs232_device::remote.gpgga.separation.value = 0;
	jvx_bitZSet(genGpsRs232_device::remote.gpgga.separation_unit.value.selection(), 1);
	genGpsRs232_device::remote.gpgga.age.value = 0;
	if (paramLst.size() >= 14)
	{
		jvxSize utcsz = jvx_string2Size(paramLst[0], err);
		std::string utcstr = jvx_size2String(utcsz);
		std::string utcval;
		jvxSize off = 0;
		if (utcstr.size() == 5)
		{
			utcval = "0" + utcstr.substr(off, 1);
			off += 1;
		}
		else if (utcstr.size() == 6)
		{
			utcval = utcstr.substr(off, 2);
			off += 2;
		}
		else
		{
			JVX_ASSERT(0);
		}
		utcval += ":";
		utcval += utcstr.substr(off, 2);
		off += 2;
		utcval += ":";
		utcval += utcstr.substr(off, 2);
		genGpsRs232_device::remote.gpgga.utc.value = utcval;

		genGpsRs232_device::remote.gpgga.latitude.value = jvx_string2Data(paramLst[1], err);
		genGpsRs232_device::remote.gpgga.longitude.value = jvx_string2Data(paramLst[3], err);
		if (paramLst[2] == "N")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.north_south.value.selection(), 0);
		}
		else if (paramLst[2] == "S")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.north_south.value.selection(), 1);
		}

		if (paramLst[4] == "E")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.east_west.value.selection(), 0);
		}
		else if (paramLst[4] == "W")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.east_west.value.selection(), 1);
		}
		
		if (paramLst[5] == "1")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.quality.value.selection(), 1);
		}
		else if (paramLst[5] == "2")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.quality.value.selection(), 2);
		}

		genGpsRs232_device::remote.gpgga.num_satellites.value = jvx_string2Size(paramLst[6], err);

		genGpsRs232_device::remote.gpgga.horizontal_dilution.value = jvx_string2Data(paramLst[7], err);
		
		genGpsRs232_device::remote.gpgga.altitude.value = jvx_string2Data(paramLst[8], err);

		if (paramLst[9] == "M")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.height_unit.value.selection(), 0);
		}
		genGpsRs232_device::remote.gpgga.separation.value = jvx_string2Data(paramLst[10], err);

		if (paramLst[11] == "M")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgga.separation_unit.value.selection(), 0);
		}

		genGpsRs232_device::remote.gpgga.age.value = jvx_string2Data(paramLst[12], err);
	}
}

void 
CjvxCuTGpsDevice::convert_nmea_gpgsa(const std::string& token)
{
	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

}

void 
CjvxCuTGpsDevice::convert_nmea_gpgll(const std::string& token)
{
	jvxSize i;
	jvxBool err = false;
	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

	/*
		Geographic Position, Latitude / Longitude and time.

		eg1.$GPGLL, 3751.65, S, 14507.36, E * 77
		eg2.$GPGLL, 4916.45, N, 12311.12, W, 225444, A

		4916.46, N    Latitude 49 deg. 16.45 min.North
		12311.12, W   Longitude 123 deg. 11.12 min.West
		225444       Fix taken at 22:54 : 44 UTC
		A            Data valid


		eg3.$GPGLL, 5133.81, N, 00042.25, W * 75
		1    2     3    4 5

		1    5133.81   Current latitude
		2    N         North / South
		3    00042.25  Current longitude
		4    W         East / West
		5 * 75       checksum

		$--GLL, lll.ll, a, yyyyy.yy, a, hhmmss.ss, A llll.ll = Latitude of position

		a = N or S
		yyyyy.yy = Longitude of position
		a = E or W
		hhmmss.ss = UTC of position
		A = status: A = valid data
		*/
#ifdef JVX_GPS_DEBUG_GPGLL

	std::cout << "=============== GPGLL: ===============" << std::endl;
	for (i = 0; i < paramLst.size(); i++)
	{
		if (i < GPGLL_MAX_NUM)
		{
			std::cout << gpgll[i] << " -- " << std::flush;
		}
		std::cout << paramLst[i] << std::endl;
	}
	std::cout << "======================================" << std::endl;
#endif

	genGpsRs232_device::remote.gpgll.latitude.value = 0;
	genGpsRs232_device::remote.gpgll.longitude.value = 0;
	jvx_bitZSet(genGpsRs232_device::remote.gpgll.north_south.value.selection(), 2);
	jvx_bitZSet(genGpsRs232_device::remote.gpgll.east_west.value.selection(), 2);
	if (paramLst.size() >= 4)
	{
		genGpsRs232_device::remote.gpgll.latitude.value = jvx_string2Data(paramLst[0], err);
		genGpsRs232_device::remote.gpgll.longitude.value = jvx_string2Data(paramLst[2], err);
		if (paramLst[1] == "N")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgll.north_south.value.selection(), 0);
		}
		if (paramLst[1] == "S")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgll.north_south.value.selection(), 1);
		}
		if (paramLst[3] == "E")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgll.east_west.value.selection(), 0);
		}
		if (paramLst[3] == "W")
		{
			jvx_bitZSet(genGpsRs232_device::remote.gpgll.east_west.value.selection(), 1);
		}
	}
}

void 
CjvxCuTGpsDevice::convert_nmea_gprmc(const std::string& token)
{
	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

}

void 
CjvxCuTGpsDevice::convert_nmea_gpgsv(const std::string& token)
{
	std::vector<std::string> paramLst;
	jvx_parseStringListIntoTokens(token, paramLst);

}
