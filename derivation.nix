{ stdenv, cmake, catch2 }:
stdenv.mkDerivation rec {
    name = "hera-${version}";
    version = "0.1.0";

    src = ./.;
    nativeBuildInputs = [ cmake catch2 ];
    buildInputs = [];

    cmakeFlags = [ "-DHERA_DOCS=OFF" "-DHERA_EXAMPLES=OFF" "-DHERA_TESTS=ON" ];

    enableParallelBuilding = true;
    releaseName = name;

    doCheck = true;
    checkTarget = "test";

    meta = with stdenv.lib; {
        description = "Heterogeneous ranges for c++20";
        homepage = https://github.com/dreyri/hera;
        license = licenses.mit;
        platforms = platforms.all;
    };
}