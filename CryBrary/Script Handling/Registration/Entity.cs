using System.Collections.Generic;

namespace CryEngine.Initialization
{
    public struct EntityRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public string category;

        public string editorHelper;
        public string editorIcon;

        public EntityClassFlags flags;

        /// <summary>
        /// EntityPropertyFolder array containing properties.
        /// The first index contains a fake folder which is referred to as the default category.
        /// </summary>
        public object[] propertyFolders;
    }
}
