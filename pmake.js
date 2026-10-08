function main(){
	const PPR = new pmake.LibraryProject("ppr");
	PPR.sources = fs.glob("src/*.c", "src/base/*.c", "src/hash/*.c", "src/misc/*.c");
	PPR.includes = ["include"];
	PPR.libraries = [];
	if(pmake.system.target == "Windows"){
		PPR.libraries.push("ws2_32");
	}else{
		PPR.libraries.push("pthread");
	}

	pmake.register(PPR);
}
