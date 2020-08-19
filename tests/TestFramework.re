include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      projectDir: "../",
      snapshotDir: "./tests/__snapshots__",
    });
}); 