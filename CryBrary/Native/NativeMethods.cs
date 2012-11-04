namespace CryEngine.Native
{
    internal static class NativeMethods
    {
        private static INative3DEngineMethods _engine3D;
        private static INativeActorMethods _actor;
        private static INativeCVarMethods _CVar;
        private static INativeDebugMethods _debug;
        private static INativeEntityMethods _entity;
        private static INativeFlowNodeMethods _flowNode;
        private static INativeGameRulesMethods _gameRules;
        private static INativeInputMethods _input;
        private static INativeItemSystemMethods _itemSystem;
        private static INativeLevelMethods _level;
        private static INativeLoggingMethods _log;
        private static INativeMaterialMethods _material;
        private static INativeParticleEffectMethods _particle;
        private static INativePhysicsMethods _physics;
        private static INativeRendererMethods _renderer;
        private static INativeScriptTableMethods _scriptTable;
        private static INativeTimeMethods _time;
        private static INativeUIMethods _UI;
        private static INativeViewMethods _view;
        private static INativeNetworkMethods _network;
        private static INativeScriptSystemMethods _scriptSystem;

        public static INative3DEngineMethods Engine3D { get { return _engine3D ?? (_engine3D = new Native3DEngineMethods()); } set { _engine3D = value; } }
        public static INativeActorMethods Actor { get { return _actor ?? (_actor = new NativeActorMethods()); } set { _actor = value; } }
        public static INativeCVarMethods CVar { get { return _CVar ?? (_CVar = new NativeCVarMethods()); } set { _CVar = value; } }
        public static INativeDebugMethods Debug { get { return _debug ?? (_debug = new NativeDebugMethods()); } set { _debug = value; } }
        public static INativeEntityMethods Entity { get { return _entity ?? (_entity = new NativeEntityMethods()); } set { _entity = value; } }
        public static INativeFlowNodeMethods FlowNode { get { return _flowNode ?? (_flowNode = new NativeFlowNodeMethods()); } set { _flowNode = value; } }
        public static INativeGameRulesMethods GameRules { get { return _gameRules ?? (_gameRules = new NativeGameRulesMethods()); } set { _gameRules = value; } }
        public static INativeInputMethods Input { get { return _input ?? (_input = new NativeInputMethods()); } set { _input = value; } }
        public static INativeItemSystemMethods ItemSystem { get { return _itemSystem ?? (_itemSystem = new NativeItemSystemMethods()); } set { _itemSystem = value; } }
        public static INativeLevelMethods Level { get { return _level ?? (_level = new NativeLevelMethods()); } set { _level = value; } }
        public static INativeLoggingMethods Log { get { return _log ?? (_log = new NativeLoggingMethods()); } set { _log = value; } }
        public static INativeMaterialMethods Material { get { return _material ?? (_material = new NativeMaterialMethods()); } set { _material = value; } }
        public static INativeParticleEffectMethods Particle { get { return _particle ?? (_particle = new NativeParticleEffectMethods()); } set { _particle = value; } }
        public static INativePhysicsMethods Physics { get { return _physics ?? (_physics = new NativePhysicsMethods()); } set { _physics = value; } }
        public static INativeRendererMethods Renderer { get { return _renderer ?? (_renderer = new NativeRendererMethods()); } set { _renderer = value; } }
        public static INativeScriptTableMethods ScriptTable { get { return _scriptTable ?? (_scriptTable = new NativeScriptTableMethods()); } set { _scriptTable = value; } }
        public static INativeTimeMethods Time { get { return _time ?? (_time = new NativeTimeMethods()); } set { _time = value; } }
        public static INativeUIMethods UI { get { return _UI ?? (_UI = new NativeUIMethods()); } set { _UI = value; } }
        public static INativeViewMethods View { get { return _view ?? (_view = new NativeViewMethods()); } set { _view = value; } }
        public static INativeNetworkMethods Network { get { return _network ?? (_network = new NativeNetworkMethods()); } set { _network = value; } }
        public static INativeScriptSystemMethods ScriptSystem { get { return _scriptSystem ?? (_scriptSystem = new NativeScriptSystemMethods()); } set { _scriptSystem = value; } }

        static NativeMethods()
        {

        }
    }
}
