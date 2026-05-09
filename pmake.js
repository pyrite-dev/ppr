function main(){
	const PPR = new pmake.LibraryProject("ppr");
	PPR.sources = fs.glob("src/*.c", "src/base/*.c", "src/hash/*.c", "src/misc/*.c");
	PPR.includes = ["include"];
	PPR.libs = ["pthread"];

	pmake.register(PPR);
}
