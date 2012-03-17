using System.Runtime.CompilerServices;

using System.Collections.ObjectModel;

using System.Linq;

namespace CryEngine
{
    public static class ViewSystem
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static uint _CreateView();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RemoveView(uint viewId);

        public static View CreateView()
        {
            Views.Add(new View(_CreateView()));

            return Views.Last();
        }

        public static void RemoveView(EntityId viewId)
        {
            var view = Views.FirstOrDefault(x => x.Id == viewId);
            if (view == default(View))
                return;

            Views.Remove(view);

            _RemoveView(viewId); 
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static uint _GetActiveView();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetActiveView(uint viewId);

        public static View ActiveView
        {
            get
            {
                var viewId = _GetActiveView();

                var view = Views.FirstOrDefault(x => x.Id == viewId);
                if (view != default(View))
                    return view;

                return new View(_GetActiveView());
            }
            set
            {
                _SetActiveView(value.Id);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static ViewParams _GetViewParams(uint viewId);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewParams(uint viewId, ViewParams cam);

        static Collection<View> Views = new Collection<View>();
    }
}