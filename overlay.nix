self: super: 
{
    hera = self.callPackage ./derivation.nix { stdenv = super.gcc10Stdenv; };
}