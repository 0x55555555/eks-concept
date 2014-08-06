import "../EksBuild" as Eks;

Eks.Library {
  name: "EksConcept"
  toRoot: "../../"

  Depends { name: "EksCore" }

  Export {
    Depends { name: "cpp" }
    Depends { name: "EksCore" }
    cpp.includePaths: ["include"]
  }
}
