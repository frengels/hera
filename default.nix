{ nixpkgs ? import <nixpkgs> {} }:
let
    nixpkgs = <nixpkgs>;
    pkgs = import nixpkgs {
        config = {};
        overlays = [
            (import ./overlay.nix)
        ];
    };
in pkgs.hera