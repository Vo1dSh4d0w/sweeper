{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...}@inputs:
  let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ];
    forAllSystems = f: nixpkgs.lib.genAttrs systems f;
    devShell =
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            gnumake
            ncurses
            pkg-config
          ];
        };
      };
    package =
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        default = pkgs.stdenv.mkDerivation {
          name = "sweeper";
          src = ./.;
          version = builtins.readFile ./version;
          buildInputs = with pkgs; [
            ncurses
            pkg-config
          ];

          CFLAGS = "-Wall -Werror -O2";

          installPhase = ''
            mkdir -p $out/bin
            cp out/sweeper $out/bin
          '';
        };
      };
  in
  {
    devShells = forAllSystems devShell;
    packages = forAllSystems package;
  };
}
