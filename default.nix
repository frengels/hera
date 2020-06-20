{
    nixpkgs ? import ./nixpkgs.nix,
    pkgs ? import nixpkgs {}
}:

pkgs.callPackage ./derivation.nix { stdenv = pkgs.gcc10Stdenv; }