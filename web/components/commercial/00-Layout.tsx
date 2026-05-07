import { Footer } from "./06-Footer";
import { Hero } from "./01-Hero";
import { FullPluginInteraction } from "./08-FullPluginInteraction";
import { ThreeDInteraction } from "./09-ThreeDInteraction";
import { MasterEnvelope } from "./10-MasterEnvelope";
import { MasterFilter } from "./11-MasterFilter";
import { Specs } from "./05-Specs";
import { AudioSamples } from "../AudioSamples";

export function CommercialLayout() {
  return (
    <>
      <Hero />
      <FullPluginInteraction />
      <ThreeDInteraction />
      <MasterEnvelope />
      <MasterFilter />
      <AudioSamples />
      <Specs />
      <Footer />
    </>
  );
}
