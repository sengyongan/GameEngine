using System;
using System.Runtime.CompilerServices;
//ʵ�����
namespace Hazel
{
	public class Entity
	{
		protected Entity() { ID = 0; } 

		internal Entity(ulong id)
		{
			ID = id;
		}

		public readonly ulong ID;//ʵ��id

        //����
		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetTranslation(ID, ref value);
			}
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new T() { Entity = this };
			return component;
		}
        public Entity FindEntityByName(string name)//ͨ��name�ҵ�ʵ��id
        {
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }

        public T As<T>() where T : Entity, new()//ASת��������ģ��T������Entity�������
        {
            object instance = InternalCalls.GetScriptInstance(ID);//ͨ��id�ҵ�����
            return instance as T;//������ת��Ϊ����
        }

    }

}
