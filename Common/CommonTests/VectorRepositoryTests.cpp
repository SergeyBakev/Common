#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;

#define SW_REDONLY_PROPERTY2(t,n, f, def)  public: __declspec( property (get = G##n)) t n; \
	public: const t& G##n()const { return f;} \
	private: t f = def;


#define SW_PROPERTY2(t,n,f,def) public: __declspec(property (put = S##n, get = G##n)) t n; \
	public: const t& G##n()const { return f;} \
	public: void S##n(const t& d) { f = d;}\
	private: t f = def;

namespace CommonTests
{
	struct UserModel
	{
		
		std::wstring Name;
		std::wstring Password;
		size_t Age = 0;
		UserModel(std::wstring name, std::wstring password, size_t age) : Name(name), Password(password), Age(age){}
		SW_PROPERTY2(_uuid_t, ID, id, _uuid_t::New());
	};
	
	class UserRepository : public VectorRepository<UserModel>
	{
	public:
		using Base = VectorRepository<UserModel>;
	    UserRepository(Storage& storage) : Base(storage) {}
		UserRepository(Storage& storage, ModelComparer compr) : Base(storage, compr)
		{

		}

		
	};

	DECLARE_PTR(UserRepository);
	DECLARE_PREFIX_PTR_(UserRepository, I);

	bool ModelPredicate(const UserModel& model1, const UserModel& model2)
	{
		return model1.Name == model2.Name;
	}

	TEST_CLASS(VectorRepositoryTests_For_Small_Model)
	{
	private:

		using UsserArray = std::vector<UserModel>;
		
		static UsserArray CreateUserArray();
		static UsserArray CreateEmptyUserArray();
		static inline 

		IUserRepositoryPtr CreateRepo(UsserArray& array)
		{
			return std::make_shared<UserRepository>(array);
		}
		
		template <class T>
		IUserRepositoryPtr CreateRepo(UsserArray& array, T predicate)
		{
			return std::make_shared<UserRepository>(array, predicate);
		}
		void AssertUserEqual(const UserModel& actual, const UserModel& expected)
		{
			Assert::AreEqual(expected.Name, actual.Name);
			Assert::AreEqual(expected.Password, actual.Password);
			Assert::AreEqual(expected.Age, actual.Age);
		}

		void AssertUserNotEqual(const UserModel& actual, const UserModel& expected)
		{
			Assert::AreNotEqual(expected.Name, actual.Name);
			Assert::AreNotEqual(expected.Password, actual.Password);
			Assert::AreNotEqual(expected.Age, actual.Age);
		}
	public:

#pragma region Get method
		TEST_METHOD(Create_And_Get_Size)
		{
			//arrange
			UsserArray uArray1 = CreateUserArray();
			const size_t UserCount = uArray1.size();
			//act
			auto repo = CreateRepo(uArray1);

			//Assert
			Assert::IsFalse(repo->IsEmpty());
			Assert::AreEqual(UserCount, repo->Count());

		}

		TEST_METHOD(Get_By_Equal_Name_Predicate)
		{
			//arange
			UsserArray uArray2 = CreateUserArray();
			const size_t UserCount = uArray2.size();
			auto repo = CreateRepo(uArray2);
			std::wstring user1Name = L"User1";
			auto byName = [&](const UserModel& user)
			{
				return user.Name == user1Name;
			};

			//act
			auto user = repo->Get(byName);

			//assert
			Assert::IsTrue((bool)user);
			Assert::AreEqual(user1Name, user->Name);
		}

		TEST_METHOD(Get_By_Equal_Name_Password)
		{
			//arange
			UsserArray uArray2 = CreateUserArray();
			const size_t UserCount = uArray2.size();
			auto repo = CreateRepo(uArray2);
			std::wstring user1Password= L"12345";
			auto byPassword = [&](const UserModel& user)
			{
				return user.Password == user1Password;
			};

			//act
			auto user = repo->Get(byPassword);

			//assert
			Assert::IsTrue((bool)user);
			Assert::AreEqual(user1Password, user->Password);
		}

		TEST_METHOD(Get_Not_Exist_User_By_Name)
		{
			//arange
			UsserArray uArray2 = CreateUserArray();
			const size_t UserCount = uArray2.size();
			auto repo = CreateRepo(uArray2);
			std::wstring user1Name = L"TestUser";
			auto byName = [&](const UserModel& user)
			{
				return user.Name == user1Name;
			};

			//act
			auto user = repo->Get(byName);

			//assert
			Assert::IsFalse((bool)user);
		}

		TEST_METHOD(Get_Not_Exist_User_By_Password)
		{
			//arange
			UsserArray uArray2 = CreateUserArray();
			const size_t UserCount = uArray2.size();
			auto repo = CreateRepo(uArray2);
			std::wstring Password = L"123dfsgfd";
			auto byPassword = [&](const UserModel& user)
			{
				return user.Password == Password;
			};

			//act
			auto user = repo->Get(byPassword);

			//assert
			Assert::IsFalse((bool)user);
		}

		TEST_METHOD(Get_By_Predicate_When_Repo_Empty)
		{
			//arange
			UsserArray array = CreateEmptyUserArray();
			auto repo = CreateRepo(array);
			std::wstring Password = L"";
			auto byPassword = [&](const UserModel& user)
			{
				return user.Password == Password;
			};

			//act
			auto user = repo->Get(byPassword);
			//assert
			
			Assert::IsFalse((bool)user);
		}

		TEST_METHOD(Get_By_Predicate_When_Repo_Empty_And_Predicate_Null)
		{
			//arange
			UsserArray array = CreateEmptyUserArray();
			auto repo = CreateRepo(array);

			//act
			auto user = repo->Get(nullptr);

			//assert
			Assert::IsFalse((bool)user);
		}

		TEST_METHOD(Get_By_Model_ID)
		{
			//arange
			UsserArray array = CreateUserArray();
			UserModel expected = array[0];
			auto repo = CreateRepo(array);

			//act
			auto user = repo->Get(expected);

			//assert
			
			Assert::IsTrue((bool)user);
			UserModel actual = *user;
			Assert::AreEqual(expected.Name, actual.Name);
			Assert::AreEqual(expected.Password, actual.Password);
			Assert::AreEqual(expected.Age, actual.Age);
		}
		
		TEST_METHOD(Get_By_Index_When_Repo_Empty)
		{
			//arange
			auto array = CreateEmptyUserArray();
			auto repo = CreateRepo(array);
			auto noExistUser = UserModel(L"User1"s, L"12345"s, 25);
			//act
			auto user = repo->Get(noExistUser);

			//assert

			Assert::IsFalse((bool)user);
		}

		TEST_METHOD(Get_By_Custom_Predicate)
		{
			//arange
			UsserArray array = CreateUserArray();
			auto repo = CreateRepo(array, ModelPredicate);
			auto& expected = array[3];
			expected.ID = _uuid_t::Nil();
			//act
			auto user = repo->Get(expected);

			//assert
			Assert::IsTrue((bool)user);
			UserModel actual = *user;
			Assert::AreEqual(expected.Name, actual.Name);
			Assert::AreEqual(expected.Password, actual.Password);
			Assert::AreEqual(expected.Age, actual.Age);
		}

#pragma endregion

#pragma region Create method

		TEST_METHOD(Add_One_User)
		{
			_uuid_t u1;
			_uuid_t u2;
			bool rc = u1 == u2;
			//arange
			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User333", L"333",87 );
			
			//act
			repo->Create(expected);
			auto actual = repo->Get(expected);

			//assert
			Assert::IsTrue((bool)actual);
			Assert::IsFalse(users.empty());
			AssertUserEqual(*actual, expected);
			AssertUserEqual(*actual, users[0]);
		}

		TEST_METHOD(Add_The_Same_User_Multiple_Times)
		{
			//arange
			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User333", L"333",87);
			UserModel fake(L"empty",L"fake",23);

			//act
			bool mark1 = repo->Create(expected);
			bool mark2 = repo->Create(expected);
			auto actual1 = repo->Get(expected);
			auto actual2 = repo->Get(fake);

			//assert
			Assert::IsTrue(mark1);
			Assert::IsTrue((bool)actual1);
			Assert::IsFalse(mark2);
			Assert::IsFalse((bool)actual2);
			Assert::AreEqual(size_t(1), repo->Count());
			Assert::AreEqual(size_t(1), users.size());
			AssertUserEqual(*actual1, expected);
			
		}

#pragma endregion

#pragma region Update method

		struct DefaultPredicate
		{
			bool operator()(const UserModel& model, const UserModel& other) const
			{
				return model.ID == other.ID;
			}
		};

		TEST_METHOD(Create_One_User_And_Update_Them)
		{
			//arange
			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User333", L"333",87 );

			//act
			repo->Create(expected);
			expected.Name = L"User123";
			expected.Password = L"567";
			expected.Age = 31;
			bool bUpdated = repo->Update(expected);
			auto actualUserState = repo->Get(expected);
			std::function<bool(const UserModel&)> f;

			f = std::bind(DefaultPredicate(), std::placeholders::_1, expected);
			//assert
			
			Assert::IsTrue((bool)actualUserState);
			Assert::IsTrue(bUpdated);
			Assert::IsFalse(users.empty());
			AssertUserEqual(*actualUserState, expected);
			AssertUserEqual(*actualUserState, users[0]);

			//assert
		}

		TEST_METHOD(Update_No_Exist_User)
		{
			//arrange
			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User333", L"333", 87);

			//act
			bool bUpdated = repo->Update(expected);

			//assert
			Assert::IsFalse(bUpdated);
			Assert::AreEqual(size_t(0), users.size());
			Assert::AreEqual(size_t(0), repo->Count());
		}

		TEST_METHOD(Create_Or_Upadte_When_User_No_Exist)
		{
			//arange

			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User56", L"333", 87);

			//act
			bool bUpdated = repo->Update(expected);

			//assert
			Assert::IsFalse(bUpdated);
			Assert::AreEqual(size_t(0), users.size());
			Assert::AreEqual(size_t(0), repo->Count());
		}

		TEST_METHOD(Create_Or_Upadte_When_User_Exist)
		{
			//arange

			std::vector<UserModel> users;
			auto repo = CreateRepo(users);
			UserModel expected(L"User56", L"333", 87);

			//act
			repo->Create(expected);
			expected.Name = L"User123";
			expected.Password = L"567";
			expected.Age = 31;
			repo->CreateOrUpdate(expected);
			auto actualUserState = repo->Get(expected);

			//assert
			Assert::IsTrue((bool)actualUserState);
			Assert::AreEqual(size_t(1), users.size());
			Assert::AreEqual(size_t(1), repo->Count());
			AssertUserEqual(*actualUserState, expected);
			AssertUserEqual(*actualUserState, users[0]);
		}


		TEST_METHOD(Update_By_Storage)
		{
			//arange
			UsserArray users = CreateUserArray();

			UserModel u1(L"Test1"s, L"1"s, 1);
			UserModel u2(L"Test2"s, L"2"s, 2);
			UserModel u3(L"Test3"s, L"3"s, 3);
			UserModel u4(L"Test4"s, L"4"s, 4);
			UsserArray users2 = { u1,u2,u3,u4};
			UsserArray usersCopy = users2;
			auto repo = CreateRepo(users);

			//act
			repo->UpdateStorage(std::move(users2));

			//assert
			auto& repoStorage = repo->GetStorage();
			Assert::IsTrue(&users == &repoStorage);
		}


#pragma endregion

#pragma region Remove method

		TEST_METHOD(Delete_Model)
		{
			//arange
			UsserArray array = CreateUserArray();
			UserModel removed = array[0];
			auto repo = CreateRepo(array);
			size_t expectedSize = repo->Count() - 1;
			size_t expectedArraySize = array.size() - 1;


			//act
			bool bDeleted = repo->Delete(removed);

			//assert

			Assert::IsTrue(bDeleted);
			Assert::AreEqual(expectedSize, repo->Count());
			Assert::AreEqual(expectedArraySize, array.size());
		}


		TEST_METHOD(Delete_Model_And_Get_By_No_Exist_Model)
		{
			//arange
			UsserArray array = CreateUserArray();
			UserModel removed = array[0];
			auto repo = CreateRepo(array);
			size_t expectedSize = repo->Count() - 1;
			size_t expectedArraySize = array.size() - 1;


			//act
			bool bDeleted = repo->Delete(removed);
			auto user = repo->Get(removed);

			//assert
			Assert::IsTrue(bDeleted);
			Assert::IsFalse((bool)user);
			Assert::AreEqual(expectedSize, repo->Count());
			Assert::AreEqual(expectedArraySize, array.size());
		}

		TEST_METHOD(Delete_The_Same_Model_Twice)
		{
			//arange
			UsserArray array = CreateUserArray();
			UserModel removed = array[0];
			auto repo = CreateRepo(array);
			size_t expectedSize = repo->Count() - 1;
			size_t expectedArraySize = array.size() - 1;


			//act
			bool bDeleted = repo->Delete(removed);
			bool bDeleted2 = repo->Delete(removed);
			

			//assert
			Assert::IsTrue(bDeleted);
			Assert::IsFalse(bDeleted2);
			Assert::AreEqual(expectedSize, repo->Count());
			Assert::AreEqual(expectedArraySize, array.size());
		}
#pragma endregion

#pragma region Action method


#pragma endregion
	};

	VectorRepositoryTests_For_Small_Model::UsserArray VectorRepositoryTests_For_Small_Model::CreateUserArray()
	{
		UserModel u1( L"User1"s,L"12345"s,25 );
		UserModel u2( L"User2"s,L"25897"s,27 );
		UserModel u3( L"User3"s,L"55555"s,3 );
		UserModel u4( L"User4"s,L"78945"s,15 );
		UserModel u5( L"User5"s,L""s,87 );
		UserModel u6( L"User6"s,L""s,87 );
		UserModel u7( L"User7"s,L"2222"s,0 );
		return{ u1,u2,u3,u4,u5,u6,u7 };
	}

	VectorRepositoryTests_For_Small_Model::UsserArray VectorRepositoryTests_For_Small_Model::CreateEmptyUserArray()
	{
		return {};
	}
}

