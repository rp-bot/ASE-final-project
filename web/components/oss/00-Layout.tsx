import { Downloads } from "./05-Downloads";
import { Footer } from "./07-Footer";
import { Hero } from "./01-Hero";
import { Internals } from "./04-Internals";
import { Readme } from "./02-Readme";
import { Showcase } from "./03-Showcase";
import { Specs } from "./06-Specs";

export function OssLayout() {
  return (
    <>
      <Hero />
      <Readme />
      <Showcase />
      <Internals />
      <Downloads />
      <Specs />
      <Footer />
    </>
  );
}
