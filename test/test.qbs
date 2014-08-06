import "../../../Eks/EksBuild" as Eks;

Eks.Test {
  name: "EksConceptTest"
  toRoot: "../../"

  Depends { name: "EksConcept" }
}
