#include <rleahylib/rleahylib.hpp>
#include <noise.hpp>
#include <random.hpp>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <random>
#include <utility>


using namespace MCPP;


Word width=50000;
Word height=256;
String filename("../simplex.txt");


enum class Biome : Byte {

	Ocean=0,
	Plains=1,
	Desert=2,
	ExtremeHills=3,
	Forest=4,
	Taiga=5,
	Swamp=6,
	River=7,
	Hell=8,
	Sky=9,
	FrozenOcean=10,
	FrozenRiver=11,
	IcePlains=12,
	IceMountains=13,
	MushroomIsland=14,
	MushroomIslandShore=15,
	Beach=16,
	DesertHills=17,
	ForestHills=18,
	TaigaHills=19,
	ExtremeHillsEdge=20,
	Jungle=21,
	JungleHills=22

};


void write_file (FILE * handle, const String & s) {

	auto c_string=s.ToCString();

	if (fputs(
		static_cast<ASCIIChar *>(c_string),
		handle
	)<0) throw std::runtime_error("Error writing to file");

}


void write_file (FILE * handle, Byte c) {

	if (fputc(
		c,
		handle
	)<0) throw std::runtime_error("Error writing to file");

}


inline Double normalize (Double lo, Double hi, Double val) noexcept {

	if (lo==hi) {
	
		if (val==lo) return 0.5;
		if (val>lo) return 1;
		return 0;
	
	}
	
	bool complement;
	if (lo>hi) {
	
		complement=true;
		std::swap(lo,hi);
	
	} else {
	
		complement=false;
	
	}
	
	Double difference=val-lo;
	Double range=hi-lo;
	
	if (difference<0) return complement ? 1 : 0;
	if (difference>range) return complement ? 0 : 1;
	
	Double result=difference/range;
	
	return complement ? 1-result : result;

}


int main () {

	auto c_filename=Path::Combine(
		Path::GetPath(
			File::GetCurrentExecutableFileName()
		),
		filename	
	).ToCString();
	
	FILE * fhandle=fopen(
		static_cast<ASCIIChar *>(
			Path::Combine(
				Path::GetPath(
					File::GetCurrentExecutableFileName()
				),
				filename	
			).ToCString()
		),
		"w"
	);
	
	if (fhandle==nullptr) {
	
		StdOut << "Could not open " << filename << " for writing" << Newline;
	
		return 1;
	
	}
	
	try {
	
		auto gen=std::mt19937_64();
		//gen.seed(CryptoRandom<std::mt19937_64::result_type>());
		gen.seed(11653);
		
		auto ocean=MakeOctave(
			Simplex(gen),
			4,
			0.2,
			0.0005
		);
		
		auto max=MakeScale(
			MakeOctave(
				Simplex(gen),
				8,
				0.2,
				0.0005
			),
			0,
			1
		);
		
		auto min=MakeScale(
			MakeBias(
				MakeOctave(
					Simplex(gen),
					4,
					0.5,
					0.00005
				),
				0.3
			),
			0,
			1
		);
		
		auto heightmap=MakeScale(
			MakeOctave(
				Simplex(gen),
				8,
				0.3,
				0.005
			),
			0,
			1
		);
		
		auto perturbate_x=MakeScale(
			MakeOctave(
				MakeGain(
					Simplex(gen),
					0.25
				),
				3,
				0.4,
				0.01
			),
			-20,
			20
		);
		
		auto perturbate_z=MakeScale(
			MakeOctave(
				MakeGain(
					Simplex(gen),
					0.25
				),
				3,
				0.4,
				0.01
			),
			-20,
			20
		);
		
		auto noise=MakeConvert<Byte>(
			MakePerturbateDomain<1>(
				MakePerturbateDomain<4>(
					[&] (Double x, Double y, Double z) {
						
						auto ocean_val=ocean(x,0,z);
						
						if (ocean_val<-0.0125) {
						
							//	Ocean
							
							//	To enable a smooth transition
							//	between sea and land, we have a
							//	"continental shelf" which slowly
							//	approaches beach level (i.e. 64)
							//	as we move towards land
							Double dampen=(ocean_val<-0.025) ? 0 : normalize(
								-0.025,
								-0.0125,
								ocean_val
							);
							
							//	Determine the "ceiling"
							//	and "floor" values
							Double ceiling=fma(
								min(x,0,z),
								16,
								76
							);
							Double floor=fma(
								max(x,0,z),
								-16,
								48
							);
							if (floor>ceiling) std::swap(floor,ceiling);
							
							//	If we're dampening, apply it
							if (dampen!=0) {
							
								floor+=dampen*(64-floor);
								ceiling+=dampen*(64-ceiling);
							
							}
							
							//	Determine current height
							return (
								(y>fma(
									heightmap(x,0),
									ceiling-floor,
									floor
								))
									//	Air or water?
									?	(
											(y>64)
												//	Air
												?	255
												//	Water
												:	127
										)
									//	Solid
									:	0
							);
						
						} else if (ocean_val<0) {
						
							//	Beach
							
							//	All beaches are at sea level
							return (y>64) ? 255 : 0;
						
						} else {
						
							//	Land
							
							//	We need to dampen the height so
							//	that everything falls off near the
							//	ocean to the beach
							Double dampen=(ocean_val<0.0125) ? normalize(
								0.0125,
								0,
								ocean_val
							) : 0;
							
							//	Determine the "ceiling" and
							//	"floor" values
							Double max_val=max(x,0);
							Double ceiling=(
								(max_val<0.2)
									?	72
									:	(
											(max_val<0.8)
												?	fma(
														normalize(
															0.2,
															0.8,
															max_val
														),
														256-72,
														72
													)
												:	256
										)
							);
							Double min_val=min(x,0);
							Double floor=(
								(min_val<0.1)
									?	fma(
											normalize(
												0,
												0.1,
												min_val
											),
											52-64,
											64
										)
									:	(
											(min_val<0.3)
												?	64
												:	(
														(min_val<0.9)
															?	fma(
																	normalize(
																		0.3,
																		0.9,
																		min_val
																	),
																	128-64,
																	64
																)
															:	128
													)
										)
							);
							if (floor>ceiling) std::swap(floor,ceiling);
							
							//	If we're dampening, apply it
							if (dampen!=0) {
							
								floor+=dampen*(64-floor);
								ceiling+=dampen*(64-ceiling);
							
							}
							
							//	Determine current height
							return (
								(y>fma(
									heightmap(x,0),
									ceiling-floor,
									floor
								))
									//	Air
									?	255
									//	Solid
									:	0
							);
						
						}
					
					}
					perturbate_y
				),
				perturbate_x
			)
		);
		
		auto noise=MakeConvert<Byte>(
			MakePerturbateDomain<1>(
				,
				MakeScale(
					MakeOctave(
						MakeGain(
							Simplex(gen),
							0.25
						),
						3,
						0.4,
						0.01
					),
					-20,
					20
				)
			)
		);
		
		String width_str(width);
		String height_str(height);
		
		write_file(
			fhandle,
			String(width_str.Size())
		);
		write_file(
			fhandle,
			width_str
		);
		write_file(
			fhandle,
			String(height_str.Size())
		);
		write_file(
			fhandle,
			height_str
		);
		
		Vector<Byte> buffer(width*height);
		
		Timer timer(Timer::CreateAndStart());
		
		for (Word x=0;x<width;++x) for (Word y=0;y<height;++y) buffer.Add(noise(x,y));
		
		timer.Stop();
		
		StdOut	<< "====GENERATION COMPLETE===="
				<< Newline
				<< "Blocks placed: "
				<< (width*height)
				<< Newline
				<< "Elapsed time: "
				<< timer.ElapsedNanoseconds()
				<< "ns"
				<< Newline
				<< "Average time per block: "
				<< (timer.ElapsedNanoseconds()/(width*height))
				<< "ns"
				<< Newline;
				
		for (Word i=0;i<buffer.Count();++i) write_file(fhandle,buffer[i]);
		
	} catch (...) {
	
		fclose(fhandle);
		
		throw;
	
	}
	
	fclose(fhandle);

	return 0;

}
