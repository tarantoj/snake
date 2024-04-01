{pkgs ? import <nixpkgs>}:
pkgs.stdenv.mkDerivation {
  pname = "snake";
  version = "0.0.1";

  src = ./.;

  buildInputs = with pkgs; [
    ncurses
  ];
  makeFlags = ["PREFIX=$(out)"];

  meta = {
    description = "snake in ncurses";
    homepage = "";
    # license = lib.licenses.;
    # maintainers = with lib.maintainers; [  ];
  };
}
