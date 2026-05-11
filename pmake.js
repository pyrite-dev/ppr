function main(){
	const PPR = new pmake.LibraryProject("ppr");
	PPR.sources = fs.glob("src/*.c", "src/base/*.c", "src/hash/*.c", "src/misc/*.c");
	PPR.includes = ["include"];
	if(pmake.system.target == "Windows"){
		PPR.libraries = ["ws2_32"];
	}else{
		PPR.libraries = ["pthread"];
	}

	pmake.register(PPR);
}
