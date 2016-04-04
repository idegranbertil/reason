/*
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * vim: set ft=rust:
 * vim: set ft=reason:
 */
/**
 * This module wraps the JS merlin service. The inputs and outputs should be
 * Reason. The internals might use JS.
 */
type service;

let nuclideClient = Js.Unsafe.js_expr "require('nuclide/pkg/nuclide-client')";

let dotCall x y z => Js.Unsafe.meth_call x y z;

let getService filePath => {
  let nuclideOCamlPathToMerlin = Atom.Config.get "nuclide.nuclide-ocaml.pathToMerlin";
  let nuclideOCamlMerlinFlags = Atom.Config.get "nuclide.nuclide-ocaml.merlinFlags";
  let nuclideOCamlPathToMerlinOverwrite = Atom.Config.get "nuclide.nuclide-ocaml.atomReasonOverwroteYour_pathToMerlin";
  let nuclideOCamlMerlinFlagsOverwrite = Atom.Config.get "nuclide.nuclide-ocaml.atomReasonOverwroteYour_merlinFlags";
  let atomReasonPathToMerlin = Atom.Config.get "AtomReason.pathToMerlin";
  let atomReasonMerlinFlags = Atom.Config.get "AtomReason.merlinFlags";
  let atomReasonMerlinLogFile = Atom.Config.get "AtomReason.merlinLogFile";
  switch atomReasonMerlinLogFile {
  | JsonString "" => ()
  | JsonString s => Atom.Env.setEnvVar "MERLIN_LOG" s
  | _ => ()
  };
  switch (nuclideOCamlPathToMerlin, nuclideOCamlPathToMerlinOverwrite) {
  | (_, Empty) =>
      Atom.Config.set
        "nuclide.nuclide-ocaml.atomReasonOverwroteYour_pathToMerlin" nuclideOCamlPathToMerlin
  | _ => ()
  };
  switch (nuclideOCamlMerlinFlags, nuclideOCamlMerlinFlagsOverwrite) {
  | (_, Empty) =>
      Atom.Config.set "nuclide.nuclide-ocaml.atomReasonOverwroteYour_merlinFlags" nuclideOCamlMerlinFlags
  | _ => ()
  };
  Atom.Config.set "nuclide.nuclide-ocaml.pathToMerlin" atomReasonPathToMerlin;
  Atom.Config.set "nuclide.nuclide-ocaml.merlinFlags" atomReasonMerlinFlags;
  Js.Unsafe.meth_call
    nuclideClient
    "getServiceByNuclideUri"
    [|Js.Unsafe.inject (Js.string "MerlinService"), Js.Unsafe.inject (Js.string filePath)|]
};

/**
 * Will turn the array into a jsArray.
 */
let runSingleCommand (service: service) (filePath: string) jsCmd andThen => {
  let onResolve result => andThen result;
  let runSingleCommandPromise =
    dotCall service "runSingleCommand" [|Js.Unsafe.inject (Js.string filePath), jsCmd|];
  dotCall runSingleCommandPromise "then" [|Js.Unsafe.inject (Js.wrap_callback onResolve)|]
};

let contextifyStringQuery cmdList filePath => Js.Unsafe.obj [|
  ("query", Js.Unsafe.inject (Js.array (Array.map Js.string (Array.of_list cmdList)))),
  ("context", Js.Unsafe.inject (Js.array [|Js.string "auto", Js.string filePath|]))
|];

let contextifyQuery cmdArray filePath => Js.Unsafe.obj [|
  ("query", Js.Unsafe.inject (Js.array cmdArray)),
  ("context", Js.Unsafe.inject (Js.array [|Js.string "auto", Js.string filePath|]))
|];