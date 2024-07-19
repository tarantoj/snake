{
  description = "A Nix-flake-based C development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    devShells = forEachSupportedSystem ({pkgs}: {
      default =
        pkgs.mkShell.override {
          stdenv = pkgs.clangStdenv;
        } {
          buildInputs = with pkgs; [
            gcc
            # make
            nil
            gnumake
            ncurses
            clang
            ccls
            alejandra
            gdb
            glibc
            glib
            cmake
            bear
            llvm
            llvmPackages.libcxxStdenv

            # debugger
            llvmPackages_latest.lldb

            # fix headers not found
            clang-tools

            # LSP and compiler
            llvmPackages.libstdcxxClang

            # other tools
            cppcheck
            llvmPackages.libllvm
            valgrind

            # stdlib for cpp
            llvmPackages.libcxx
          ];
        };
    });
    packages = forEachSupportedSystem ({pkgs}: {default = pkgs.callPackage ./package.nix {};});
  };
}
