{
  description = "A linux driver for steelseries mouse.";

  inputs = {
    nixpkgs = {
      type = "github";
      owner = "NixOs";
      repo = "nixpkgs";
    };

    flake-utils = {
      type = "github";
      owner = "numtide";
      repo = "flake-utils";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          stdenv = with pkgs; overrideCC clangStdenv [ gcc ];
        in
        rec {
          packages = flake-utils.lib.flattenTree {
            steelseries-engine = stdenv.mkDerivation {
              pname = "steelseries-engine";
              version = "1.0";
              src = ./.;
              buildInputs = [ ];
              nativeBuildInputs = with pkgs; [
                hidapi
              ];
            };
          };

          defaultPackage = packages.steelseries-engine;
          apps.hello = flake-utils.lib.mkApp { drv = packages.steelseries-engine; };
          defaultApp = apps.hello;

          devShell = pkgs.mkShell.override { inherit stdenv; } {
            inputsFrom = [ packages.steelseries-engine ];
            packages = with pkgs; [
              usbutils
              wireshark
            ];

            # -02 enforces -D_FORTIFY_SOURCE=2 which breaks debugging
            hardeningDisabble = [ "all" ];
          };
        }
      );
}
