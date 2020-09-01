{
    description = "Heterogeneous ranges for c++20";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs";
    };

    outputs = { self, nixpkgs }:
      let
        allSystems = [ "x86_64-linux" "i686-linux" "aarch64-linux" ];
        forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f system);
        nixpkgsFor = forAllSystems (system:
          import nixpkgs {
            inherit system;
            overlays = [ self.overlay ];
          }
        );

        heraCfg = final: stdenv: stdenv.mkDerivation {
          name = "hera";
            
          nativeBuildInputs = [ final.cmake final.catch2 ];
          buildInputs = [];

          src = self;

          cmakeFlags = [ 
            "-DHERA_DOCS=OFF"
            "-DHERA_EXAMPLES=OFF"
            "-DHERA_TESTS=ON"
          ];

          enableParallelBuilding = true;

          doCheck = true;
          checkTarget = "test";

          meta = with stdenv.lib; {
            description = self.description;
            homepage = "https://github.com/dreyri/hera";
            license = licenses.mit;
            platforms = platforms.all;
          };
        };

      in {
        overlay = final: prev: 
          let
            clang10Stdenv = final.overrideCC final.clangStdenv final.clang_10;
          in {
          heraClang = heraCfg final final.clangStdenv;
          heraClang10 = heraCfg final clang10Stdenv;
          heraGcc10 = heraCfg final final.gcc10Stdenv;

          hera = heraCfg final final.gcc10Stdenv;
        };

        packages = forAllSystems (system: {
          inherit (nixpkgsFor.${system}) hera heraGcc10 heraClang heraClang10;
        });

        defaultPackage = forAllSystems (system: self.packages.${system}.hera);
      };
}