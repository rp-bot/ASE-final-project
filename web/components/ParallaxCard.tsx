"use client";

import {
  motion,
  useMotionValue,
  useSpring,
  useTransform,
  type MotionValue,
} from "framer-motion";
import {
  createContext,
  useCallback,
  useContext,
  useRef,
  type ReactNode,
} from "react";

type ParallaxContextValue = {
  x: MotionValue<number>;
  y: MotionValue<number>;
};
const ParallaxContext = createContext<ParallaxContextValue | null>(null);

type ParallaxCardProps = {
  children: ReactNode;
  className?: string;
  maxTiltDeg?: number;
  perspective?: number;
};

export function ParallaxCard({
  children,
  className = "",
  maxTiltDeg = 7,
  perspective = 1400,
}: ParallaxCardProps) {
  const ref = useRef<HTMLDivElement>(null);
  const x = useMotionValue(0);
  const y = useMotionValue(0);

  const rotateX = useSpring(useTransform(y, [-0.5, 0.5], [maxTiltDeg, -maxTiltDeg]), {
    stiffness: 160,
    damping: 20,
    mass: 0.4,
  });
  const rotateY = useSpring(useTransform(x, [-0.5, 0.5], [-maxTiltDeg, maxTiltDeg]), {
    stiffness: 160,
    damping: 20,
    mass: 0.4,
  });

  const handleMove = useCallback(
    (e: React.MouseEvent) => {
      const el = ref.current;
      if (!el) return;
      const rect = el.getBoundingClientRect();
      x.set((e.clientX - rect.left) / rect.width - 0.5);
      y.set((e.clientY - rect.top) / rect.height - 0.5);
    },
    [x, y],
  );
  const handleLeave = useCallback(() => {
    x.set(0);
    y.set(0);
  }, [x, y]);

  return (
    <ParallaxContext.Provider value={{ x, y }}>
      <div
        ref={ref}
        onMouseMove={handleMove}
        onMouseLeave={handleLeave}
        style={{ perspective }}
        className={className}
      >
        <motion.div
          style={{
            rotateX,
            rotateY,
            transformStyle: "preserve-3d",
          }}
          className="relative will-change-transform"
        >
          {children}
        </motion.div>
      </div>
    </ParallaxContext.Provider>
  );
}

export function ParallaxLayer({
  depth = 0,
  children,
  className = "",
}: {
  depth?: number;
  children: ReactNode;
  className?: string;
}) {
  const ctx = useContext(ParallaxContext);
  const fallback = useMotionValue(0);
  const offsetX = useTransform(ctx?.x ?? fallback, (v) => v * depth * 40);
  const offsetY = useTransform(ctx?.y ?? fallback, (v) => v * depth * 40);
  return (
    <motion.div
      style={{
        x: offsetX,
        y: offsetY,
        translateZ: depth * 30,
        transformStyle: "preserve-3d",
      }}
      className={className}
    >
      {children}
    </motion.div>
  );
}
