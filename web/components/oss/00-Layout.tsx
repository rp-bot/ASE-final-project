import { Downloads } from "./05-Downloads";
import { Footer } from "./07-Footer";
import { Hero } from "./01-Hero";
import { Internals } from "./04-Internals";
import { FullPluginInteraction } from "./08-FullPluginInteraction";
import { MasterEnvelope } from "./10-MasterEnvelope";
import { MasterFilter } from "./11-MasterFilter";
import { Readme } from "./02-Readme";
import { Specs } from "./06-Specs";
import { ThreeDInteraction } from "./09-ThreeDInteraction";
import { AudioSamples } from "../AudioSamples";

export function OssLayout() {
  return (
    <>
      <Hero />
      <Readme />
      <FullPluginInteraction />
      <ThreeDInteraction />
      <MasterEnvelope />
      <MasterFilter />
      {/* <Internals /> */}
      <AudioSamples />
      <Specs />
      <Downloads />
      <Footer />
    </>
  );
}
