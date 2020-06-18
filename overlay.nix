self: super: 
{
    hera = self.callPackage ./derivation.nix { stdenv = (self.overrideCC self.stdenv self.pkgs.gcc10); };
}