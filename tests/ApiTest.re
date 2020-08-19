open TestFramework;
// open ReDiscord__Api;

describe("API Tests", ({test, _}) => {
  test("Base URI to be constructed correctly", ({expect, _}) => {
    let uri = "test";

    expect.string(uri).toEqual("test");
  })
}); 