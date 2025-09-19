{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...}@inputs:
  let
    systems = [
      "x86_64_linux"
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
  in
  {
    devShells = forAllSystems devShell;
  };
}
